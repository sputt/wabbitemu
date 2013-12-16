using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Symbols;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services
{
	public sealed class WabbitcodeDebugger : IWabbitcodeDebugger
    {
        #region Private Fields

	    private byte _appPage;
	    private ushort _oldSP;
		private IWabbitemuDebugger _debugger;
	    private bool _disposed;
		private readonly List<KeyValuePair<ushort, ushort>> _memoryAllocations;
		
        private IBreakpoint _stepOverBreakpoint;
        private IBreakpoint _stepOutBreakpoint;
		private IBreakpoint _jforceBreakpoint;
		private IBreakpoint _ramClearBreakpoint;
		private IBreakpoint _insertMemBreakpoint;
		private IBreakpoint _delMemBreakpoint;

		private readonly ISymbolService _symbolService;
        private readonly IFileReaderService _fileReaderService;
	    private readonly IDebuggerService _debuggerService;

	    #endregion

        #region Constants

        private const ushort RamCode = 0x8100;
	    private const ushort TopStackApp = 0xFFDF;
	    private const ushort MachineStackBottom = 0xFE66;

	    #endregion

        #region Public Properties

	    private string CurrentDebuggingFile { get; set; }

        public IZ80 CPU
		{
			get
			{
				return _debugger.CPU;
			}
		}

        public Image ScreenImage
        {
            get { return _debugger.GetScreenImage(); }
        }

	    private bool IsAnApp { get; set; }

	    public bool IsRunning
		{
			get
			{
			    return _debugger != null && _debugger.Running;
			}
		}

	    public Stack<StackEntry> MachineStack { get; private set; }

        public Stack<CallStackEntry> CallStack { get; private set; }

	    #endregion

		#region Events

	    public event DebuggerRunning DebuggerRunningChanged;
	    public event DebuggerStep DebuggerStep;

		#endregion

		public WabbitcodeDebugger(string outputFile)
		{
		    _disposed = false;

		    _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
		    _fileReaderService = ServiceFactory.Instance.GetServiceInstance<IFileReaderService>();
			_symbolService = ServiceFactory.Instance.GetServiceInstance<ISymbolService>();

            WabbitcodeBreakpointManager.OnBreakpointAdded += WabbitcodeBreakpointManager_OnBreakpointAdded;
            WabbitcodeBreakpointManager.OnBreakpointRemoved += WabbitcodeBreakpointManager_OnBreakpointRemoved;

            _debugger = new WabbitemuDebugger();
            _debugger.LoadFile(outputFile);
            _debugger.Visible = true;
            _debugger.OnBreakpoint += BreakpointHit;
            _debugger.OnClose += DebuggerOnClose;

            CurrentDebuggingFile = outputFile;
            IsAnApp = outputFile.EndsWith(".8xk");
            _memoryAllocations = new List<KeyValuePair<ushort, ushort>>();
            CallStack = new Stack<CallStackEntry>();
            MachineStack = new Stack<StackEntry>();
            _oldSP = IsAnApp ? TopStackApp : (ushort)0xFFFF;
            SetupInternalBreakpoints();
		}

        #region Memory and Paging

        public byte ReadByte(ushort address)
        {
            return _debugger.Memory.ReadByte(address);
        }

        public ushort ReadShort(ushort address)
        {
            return _debugger.Memory.ReadWord(address);
        }

	    public ushort? GetRegisterValue(string wordHovered)
	    {
	        if (_debugger == null)
	        {
	            return null;
	        }

	        switch (wordHovered.Trim().ToLower())
	        {
                case "a":
	                return _debugger.CPU.A;
                case "f":
	                return _debugger.CPU.F;
                case "b":
	                return _debugger.CPU.B;
                case "c":
	                return _debugger.CPU.C;
                case "d":
	                return _debugger.CPU.D;
                case "e":
	                return _debugger.CPU.E;
                case "h":
                    return _debugger.CPU.H;
                case "l":
                    return _debugger.CPU.L;
                case "ixh":
                    return _debugger.CPU.IXH;
                case "ixl":
                    return _debugger.CPU.IXL;
                case "iyh":
                    return _debugger.CPU.IYH;
                case "iyl":
                    return _debugger.CPU.IYL;
                case "af":
                    return _debugger.CPU.AF;
                case "bc":
                    return _debugger.CPU.BC;
                case "de":
                    return _debugger.CPU.DE;
                case "hl":
                    return _debugger.CPU.HL;
                case "ix":
                    return _debugger.CPU.IX;
                case "iy":
                    return _debugger.CPU.IY;
                case "sp":
                    return _debugger.CPU.SP;
                case "pc":
                    return _debugger.CPU.PC;
                default:
	                return null;
	        }
	    }

	    private byte GetRelativePageNum(ushort address)
		{
			IPage bank = _debugger.Memory.Bank[address >> 14];
			int page = bank.Index;
			if (bank.IsFlash)
			{
				page = _appPage - page;
			}
			return (byte)page;
		}

		private byte GetAbsolutePageNum(ushort address)
		{
			IPage bank = _debugger.Memory.Bank[address >> 14];
			int page = bank.Index;
			return (byte)page;
		}

        public void GotoAddress(ushort address)
        {
            int page = GetRelativePageNum(address);
            DocumentLocation key = _symbolService.ListTable.GetFileLocation(page, address, address >= 0x8000);
            if (key == null)
            {
                return;
            }

            new GotoLineAction(key).Execute();
        }

        #endregion

        #region Startup

        public void EndDebug()
        {
            IsAnApp = false;

            if (_debugger == null)
            {
                return;
            }

            _debugger.EndDebug();
            _debugger = null;
        }

        private void DebuggerOnClose(object sender, EventArgs eventArgs)
        {
            _debuggerService.EndDebugging();
        }


        #endregion

        #region Running

        public void Run()
        {
            if (_debugger == null)
            {
                return;
            }

            _debugger.Step();
            _debugger.Running = true;

            if (DebuggerRunningChanged != null)
            {
                DebuggerRunningChanged(this, new DebuggerRunningEventArgs(null, true));
            }
        }

		public void Pause()
		{
			_debugger.Running = false;
			ushort currentPC = _debugger.CPU.PC;
			int maxStep = 500;
			DocumentLocation key = _symbolService.ListTable.GetFileLocation(currentPC, GetRelativePageNum(currentPC), currentPC >= 0x8000);
			while (key == null && maxStep >= 0)
			{
				_debugger.Step();
				currentPC = _debugger.CPU.PC;
				key = _symbolService.ListTable.GetFileLocation(GetRelativePageNum(currentPC), currentPC, currentPC >= 0x8000);
				maxStep--;
			}

			if (maxStep < 0)
			{
				throw new DebuggingException("Unable to pause here");
			}

			if (DebuggerRunningChanged != null)
			{
				DebuggerRunningChanged(this, new DebuggerRunningEventArgs(key, false));
			}
		}

        public void SetPCToSelect(string fileName, int lineNumber)
        {
            CalcLocation value = _symbolService.ListTable.GetCalcLocation(fileName, lineNumber);
            if (value == null)
            {
                throw new Exception("Unable to set statement here!");
            }

            _debugger.CPU.PC = value.Address;
            byte page = value.Page;
            if (IsAnApp)
            {
                page = (byte)(_appPage - page);
                _debugger.Memory.Bank[1] = _debugger.Memory.Flash[page];
            }
            else
            {
                _debugger.Memory.Bank[2] = _debugger.Memory.RAM[1];
            }

            if (DebuggerStep != null)
            {
                DebuggerStep(this, new DebuggerStepEventArgs(new DocumentLocation(fileName, lineNumber)));
            }
        }

	    public void StartDebug()
	    {
	        var app = VerifyApp(CurrentDebuggingFile);
            // once we have the app we can add breakpoints
            var breakpoints = WabbitcodeBreakpointManager.Breakpoints.ToList();
            foreach (WabbitcodeBreakpoint breakpoint in breakpoints)
            {
                SetBreakpoint(breakpoint);
            }

	        if (app != null)
	        {
	            LaunchApp(app.Value.Name);
	        }
	    }

	    public void Step()
	    {
	        // need to clear the old breakpoint so lets save it
	        ushort currentPC = _debugger.CPU.PC;
	        byte oldPage = GetRelativePageNum(currentPC);
	        DocumentLocation key = _symbolService.ListTable.GetFileLocation(oldPage, currentPC, currentPC >= 0x8000);
	        DocumentLocation newKey = key;
	        while (newKey == null || newKey.LineNumber == key.LineNumber)
	        {
	            _debugger.Step();
	            newKey = _symbolService.ListTable.GetFileLocation(GetRelativePageNum(_debugger.CPU.PC), _debugger.CPU.PC, _debugger.CPU.PC >= 0x8000);
	            // we are safe to check this here, because we are stepping one at a time meaning if the stack did change, it can't have changed much
	            if (_oldSP != _debugger.CPU.SP)
	            {
	                UpdateStack();
	            }
	        }

	        ushort address = _debugger.CPU.PC;
	        byte page = GetRelativePageNum(address);
	        key = _symbolService.ListTable.GetFileLocation(page, address, address >= 0x8000);

	        if (DebuggerStep != null)
            {
                DebuggerStep(this, new DebuggerStepEventArgs(key));
            }
        }

        public void StepOut()
        {
            DocumentLocation lastCallLocation = CallStack.Last().CallLocation;
            CalcLocation calcLocation = _symbolService.ListTable.GetCalcLocation(lastCallLocation.FileName, lastCallLocation.LineNumber);
            DocumentLocation docLocation = null;
            ushort address = calcLocation.Address;
            while (docLocation == null)
            {
                address++;
                docLocation = _symbolService.ListTable.GetFileLocation(calcLocation.Page, address, calcLocation.IsRam);
            }
            _stepOutBreakpoint = _debugger.SetBreakpoint(calcLocation.IsRam, (byte) (_appPage - calcLocation.Page), address);
            _debugger.OnBreakpoint += StepOutBreakpointEvent;
            _debugger.Step();
            _debugger.Running = true;
        }

	    private void StepOutBreakpointEvent(object sender, BreakpointEventArgs breakpointEventArgs)
	    {
            _debugger.ClearBreakpoint(_stepOutBreakpoint);
	        int page = GetRelativePageNum(_stepOutBreakpoint.Address.Address);
            DocumentLocation key = _symbolService.ListTable.GetFileLocation(page,
                _stepOutBreakpoint.Address.Address,
                !_stepOutBreakpoint.Address.Page.IsFlash);

            UpdateStack();
            _debugger.OnBreakpoint -= StepOutBreakpointEvent;

            if (DebuggerStep != null)
            {
                DebuggerStep(this, new DebuggerStepEventArgs(key));
            }
	    }

	    public void StepOver()
        {
            // need to clear the old breakpoint so lets save it
            ushort currentPC = _debugger.CPU.PC;
            byte oldPage = GetRelativePageNum(currentPC);
            DocumentLocation key = _symbolService.ListTable.GetFileLocation(oldPage, currentPC, currentPC >= 0x8000);

            string line = _fileReaderService.GetLine(key.FileName, key.LineNumber);

            int commentIndex = line.IndexOf(";");
            if (commentIndex != -1)
            {
                line = line.Substring(0, commentIndex);
            }

            // if the line contains a special commmand (i.e. one that will go to who knows where)
            // we just want to step over it
            string[] specialCommands = { "jp", "jr", "ret", "djnz" };
            if (specialCommands.Any(s => line.Contains(s)))
            {
                Step();
                return;
            }

            do
            {
                currentPC++;
                oldPage = GetRelativePageNum(currentPC);
                key = _symbolService.ListTable.GetFileLocation(oldPage, currentPC, currentPC >= 0x8000);
            } while (key == null);

            _stepOverBreakpoint = _debugger.SetBreakpoint(currentPC >= 0x8000, GetAbsolutePageNum(currentPC), currentPC);
            _debugger.OnBreakpoint += StepOverBreakpointEvent;

            _debugger.Step();
            _debugger.Running = true;
        }

        private void StepOverBreakpointEvent(object sender, EventArgs e)
        {
            _debugger.ClearBreakpoint(_stepOverBreakpoint);
            int page = _stepOverBreakpoint.Address.Page.IsFlash
                ? _appPage - _stepOverBreakpoint.Address.Page.Index
                : _stepOverBreakpoint.Address.Page.Index;
            DocumentLocation key = _symbolService.ListTable.GetFileLocation(page,
                _stepOverBreakpoint.Address.Address,
                !_stepOverBreakpoint.Address.Page.IsFlash);

            UpdateStack();
            _debugger.OnBreakpoint -= StepOverBreakpointEvent;

            if (DebuggerStep != null)
            {
                DebuggerStep(this, new DebuggerStepEventArgs(key));
            }
        }

        #endregion

        #region MachineStack

        private void UpdateCallstack()
        {
            CallStack = new Stack<CallStackEntry>(
                MachineStack.Where(s => s.CallStackEntry != null)
                .Select(s => s.CallStackEntry).Reverse());
        }

	    private CallStackEntry CheckValidCall(int currentSP)
	    {
	        DocumentLocation location;
	        Match match = CheckValidCall(ReadShort((ushort) currentSP), out location);
	        if (match == null)
	        {
	            return null;
	        }

	        string callTypeString = match.Groups["command"].Value + " " + match.Groups["condition"].Value;
	        string nameString = match.Groups["call"].Value;
	        return new CallStackEntry(callTypeString, nameString, location);
	    }

        private Match CheckValidCall(ushort stackData, out DocumentLocation location)
        {
            location = null;
            int page = GetRelativePageNum(stackData);
            if (IsAnApp && (stackData < 0x4000 || stackData >= 0x8000))
            {
                return null;
            }

            DocumentLocation key;
            do
            {
                key = _symbolService.ListTable.GetFileLocation(page, --stackData, stackData >= 0x8000);
            } while (key == null);
            string line = _fileReaderService.GetLine(key.FileName, key.LineNumber);
            Regex callRegex = new Regex(@"\s*(?<command>\w*call)[\(?|\s]\s*((?<condition>z|nz|c|nc),\s*)?(?<call>\w*?)\)?\s*(;.*)?$",
                RegexOptions.Compiled | RegexOptions.IgnoreCase);
            Match match = callRegex.Match(line);
            if (!match.Success)
            {
                return null;
            }

            location = key;
            return match;
        }

	    private void UpdateStack()
	    {
	        var oldStackList = MachineStack.Reverse().ToList();
            MachineStack.Clear();
            int currentSP = _debugger.CPU.SP;
            ushort topStack = IsAnApp ? TopStackApp : (ushort) 0xFFFF;
            // avoid generating a massive callstack
            if (currentSP < MachineStackBottom)
            {
                int maxStackSize = topStack - MachineStackBottom;
                topStack = (ushort) (currentSP + maxStackSize);
            }

	        if ((currentSP < _oldSP) || (currentSP < topStack && oldStackList.Count == 0))
	        {
                // new stack entries to add
	            while (currentSP != _oldSP && currentSP <= topStack)
	            {
                    CallStackEntry callStackEntry = CheckValidCall(currentSP);
                    MachineStack.Push(new StackEntry((ushort) currentSP, ReadShort((ushort) currentSP), callStackEntry));
	                currentSP += 2;
	            }
	        }
	        else if (currentSP > _oldSP)
	        {
	            // stack entries to remove
	            oldStackList.RemoveAll(s => s.Address < currentSP);
	        }

	        foreach (StackEntry stackEntry in oldStackList)
	        {
	            int data = ReadShort((ushort) currentSP);
	            if (stackEntry.Data != data)
	            {
	                CallStackEntry callStackEntry = CheckValidCall(currentSP);
                    MachineStack.Push(new StackEntry((ushort) currentSP, (ushort) data, callStackEntry));
	            }
	            else
	            {
	                MachineStack.Push(stackEntry);
	            }
                currentSP += 2;
	        }
            _oldSP = _debugger.CPU.SP;

            UpdateCallstack();
	    }

	    #endregion

        #region Breakpoints

        public bool SetBreakpoint(WabbitcodeBreakpoint newBreakpoint)
		{
			if (_debugger == null || newBreakpoint == null)
			{
				return false;
			}

			CalcLocation location = _symbolService.ListTable.GetCalcLocation(newBreakpoint.File, newBreakpoint.LineNumber + 1);
			if (location == null)
			{
				// move the breakpoint to the nearest location
				string fileName = newBreakpoint.File;
				int lineNumber = newBreakpoint.LineNumber;
				CalcLocation value = _symbolService.ListTable.GetNextNearestCalcLocation(fileName, lineNumber + 1);
				DocumentLocation newLocation = _symbolService.ListTable.GetFileLocation(value.Page, value.Address, value.IsRam);
			    WabbitcodeBreakpointManager.RemoveBreakpoint(fileName, lineNumber);
			    WabbitcodeBreakpointManager.AddBreakpoint(newLocation.FileName, newLocation.LineNumber - 1);
				return true;
			}

			newBreakpoint.Page = location.Page;
			newBreakpoint.Address = location.Address;
			newBreakpoint.IsRam = location.IsRam;
			newBreakpoint.WabbitemuBreakpoint = _debugger.SetBreakpoint(newBreakpoint.IsRam,
				(byte) (_appPage - newBreakpoint.Page), newBreakpoint.Address);
		    return false;
		}

		public void ClearBreakpoint(WabbitcodeBreakpoint newBreakpoint)
		{
            if (_debugger != null && newBreakpoint != null && newBreakpoint.WabbitemuBreakpoint != null)
			{
				_debugger.ClearBreakpoint(newBreakpoint.WabbitemuBreakpoint);
			}
		}

        private void BreakpointHit(object sender, BreakpointEventArgs e)
        {
            IBreakpoint breakEvent = e.Breakpoint;
            if ((breakEvent.Address == _jforceBreakpoint.Address && breakEvent.Address.Page == _jforceBreakpoint.Address.Page) ||
                (breakEvent.Address == _ramClearBreakpoint.Address && breakEvent.Address.Page == _ramClearBreakpoint.Address.Page))
            {
                DebuggerOnClose(null, EventArgs.Empty);
                return;
            }

            if (breakEvent.Address == _insertMemBreakpoint.Address && breakEvent.Address.Page == _insertMemBreakpoint.Address.Page)
            {
                _memoryAllocations.Add(new KeyValuePair<ushort, ushort>(_debugger.CPU.DE, _debugger.CPU.HL));
                _debugger.Step();
                _debugger.Running = true;
                return;
            }

            if (breakEvent.Address == _delMemBreakpoint.Address && breakEvent.Address.Page == _delMemBreakpoint.Address.Page)
            {
                _memoryAllocations.RemoveAll(kvp => kvp.Key == _debugger.CPU.HL && kvp.Value == _debugger.CPU.DE);
                _debugger.Step();
                _debugger.Running = true;
                return;
            }

            ushort address = breakEvent.Address.Address;
            int relativePage = GetRelativePageNum(address);
            WabbitcodeBreakpoint breakpoint = WabbitcodeBreakpointManager.Breakpoints.FirstOrDefault(
                b => b.Address == address && b.Page == (byte)relativePage && b.IsRam == address >= 0x8000);
            if (breakpoint == null)
            {
                return;
            }

            breakpoint.NumberOfTimesHit++;
            bool conditionsTrue = breakpoint.Enabled;
            switch (breakpoint.HitCountCondition)
            {
                case HitCountEnum.BreakEqualTo:
                    if (breakpoint.NumberOfTimesHit != breakpoint.HitCountConditionNumber)
                    {
                        conditionsTrue = false;
                    }
                    break;
                case HitCountEnum.BreakGreaterThanEqualTo:
                    if (breakpoint.NumberOfTimesHit < breakpoint.HitCountConditionNumber)
                    {
                        conditionsTrue = false;
                    }
                    break;
                case HitCountEnum.BreakMultipleOf:
                    if (breakpoint.NumberOfTimesHit % breakpoint.HitCountConditionNumber != 0)
                    {
                        conditionsTrue = false;
                    }
                    break;
            }

            if (conditionsTrue && breakpoint.EvalulateAllConditions(_debugger.CPU))
            {
                DocumentLocation key = _symbolService.ListTable.GetFileLocation(relativePage, address, !breakEvent.Address.Page.IsFlash);
                if (key == null)
                {
                    throw new InvalidOperationException("Unable to find breakpoint");
                }

                UpdateStack();

                if (DebuggerRunningChanged != null)
                {
                    DebuggerRunningChanged(this, new DebuggerRunningEventArgs(key, false));
                }
            }
            else
            {
                _debugger.Running = true;
            }
        }

	    private void SetupInternalBreakpoints()
        {
            if (_debugger == null)
            {
                return;
            }

            // this is the start _JForceCmdNoChar
            const ushort jforceCmdNoChar = 0x4027;
            const ushort insertMem = 0x42F7;
            const ushort delMem = 0x4357;
            CalcLocation location = LookupBcallAddress(jforceCmdNoChar);
            _jforceBreakpoint = _debugger.SetBreakpoint(location.IsRam, location.Page, location.Address);
            // most likely location that a crash will end up
            _ramClearBreakpoint = _debugger.SetBreakpoint(false, 0, 0x0000);
            // for restarts we want to manually delmem
            location = LookupBcallAddress(insertMem);
            _insertMemBreakpoint = _debugger.SetBreakpoint(location.IsRam, location.Page, location.Address);
            // we need to track any memory freed as well
            location = LookupBcallAddress(delMem);
            _delMemBreakpoint = _debugger.SetBreakpoint(location.IsRam, location.Page, location.Address);
        }

	    private void WabbitcodeBreakpointManager_OnBreakpointRemoved(object sender, WabbitcodeBreakpointEventArgs e)
        {
            ClearBreakpoint(e.Breakpoint);
        }

        void WabbitcodeBreakpointManager_OnBreakpointAdded(object sender, WabbitcodeBreakpointEventArgs e)
        {
            e.Cancel = SetBreakpoint(e.Breakpoint);
        }

        #endregion

        #region TIOS Specifics

        /// <summary>
		/// Returns the location of the actual code a bcall will execute.
		/// This is what TIOS does when you do a bcall.
		/// </summary>
		/// <param name="bcallAddress">The address of the bcall to map</param>
		/// <returns>The address the bcall code is located at</returns>
		private CalcLocation LookupBcallAddress(int bcallAddress)
		{
			int page;
			if ((bcallAddress & (1 << 15)) != 0)
			{
				bcallAddress &= ~(1 << 15);
				switch (_debugger.Model)
				{
					case Calc_Model.TI_73:
					case Calc_Model.TI_83P:
						page = 0x1F;
						break;
					case Calc_Model.TI_83PSE:
					case Calc_Model.TI_84PSE:
						page = 0x7F;
						break;
					case Calc_Model.TI_84P:
						page = 0x3F;
						break;
					default:
						throw new InvalidOperationException("Invalid model");
				}
			}
			else if ((bcallAddress & (1 << 14)) != 0)
			{
				bcallAddress &= ~(1 << 14);
				switch (_debugger.Model)
				{
					case Calc_Model.TI_73:
					case Calc_Model.TI_83P:
						page = 0x1B;
						break;
					case Calc_Model.TI_83PSE:
					case Calc_Model.TI_84PSE:
						page = 0x7B;
						break;
					case Calc_Model.TI_84P:
						page = 0x3B;
						break;
					default:
						throw new InvalidOperationException("Invalid model");
				}
			}
			else
			{
				throw new InvalidOperationException("Tried looking up a local bcall");
			}

			bcallAddress += 0x4000;
			ushort realAddress = _debugger.Memory.Flash[page].ReadWord((ushort)bcallAddress);
			byte realPage = _debugger.Memory.Flash[page].ReadByte((ushort)(bcallAddress + 2));
			return new CalcLocation(realAddress, realPage, false);
		}

	    private TIApplication? VerifyApp(string createdName)
		{
			if (_debugger == null || _debugger.Apps.Length == 0)
			{
				throw new DebuggingException("Application not found on calculator");
			}

			char[] buffer = new char[8];
	        using (StreamReader appReader = new StreamReader(createdName))
	        {
	            for (int i = 0; i < 17; i++)
	            {
	                appReader.Read();
	            }

	            appReader.ReadBlock(buffer, 0, 8);
	        }

	        string appName = new string(buffer);
			TIApplication? app = _debugger.Apps.Cast<TIApplication>().SingleOrDefault(a => a.Name == appName);
			if (app == null || string.IsNullOrEmpty(app.Value.Name))
			{
				throw new DebuggingException("Application not found on calculator");
			}

			_appPage = (byte)app.Value.Page.Index;
			return app;
		}

	    private void LaunchApp(string createdName)
		{
			const ushort progToEdit = 0x84BF;
			// this is code to do
			// bcall(_CloseEditBuf)
			// bcall(_ExecuteApp)
			byte[] launchAppCode = { 0xEF, 0xD3, 0x48, 0xEF, 0x51, 0x4C};
			byte[] createdNameBytes = System.Text.Encoding.ASCII.GetBytes(createdName);
			// _ExecuteApp expects the name of the app to launch in progToEdit
			_debugger.Running = false;
			_debugger.Write(true, 1, progToEdit, createdNameBytes);
			_debugger.Write(true, 1, RamCode, launchAppCode);
			_debugger.CPU.Halt = false;
			_debugger.CPU.PC = RamCode;
			_debugger.Running = true;
		}

	    #endregion

        #region IDisposable

        public void Dispose()
		{
			Dispose(true);
		}

		private void Dispose(bool disposing)
		{
			if (!_disposed)
			{
				if (disposing)
				{
                    if (_debugger != null)
                    {
                        _debugger.Dispose();
                    }
				}
			}
			_disposed = true;
        }

        #endregion
    }
}