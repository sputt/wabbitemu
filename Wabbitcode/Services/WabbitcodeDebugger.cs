using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Services.Symbols;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows.Forms;
using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services
{
	public sealed class WabbitcodeDebugger : IWabbitcodeDebugger, IDisposable
	{
		private byte _appPage;
		private readonly List<WabbitcodeBreakpoint> _breakpoints = new List<WabbitcodeBreakpoint>();
		private IWabbitemuDebugger _debugger;
		private bool _isAnApp;
		private bool _isBreakpointed;
		private readonly Stack<int> _stepStack = new Stack<int>();
		private bool _disposed;
		private IBreakpoint _stepOverBreakpoint;
		private int _stepOverLineNumber;
		private readonly ISymbolService _symbolService;
		private readonly IDocumentService _documentService;

		#region Public Properties

		public byte AppPage
		{
			get
			{
				return _appPage;
			}
			private set
			{
				_appPage = value;
			}
		}

		private Array Apps
		{
			get
			{
				return _debugger.Apps;
			}
		}

		public List<WabbitcodeBreakpoint> Breakpoints
		{
			get
			{
				return _breakpoints;
			}
		}

		public IZ80 CPU
		{
			get
			{
				return _debugger.CPU;
			}
		}

		public bool IsAnApp
		{
			get
			{
				return _isAnApp;
			}
		}

		public bool IsBreakpointed
		{
			get
			{
				return _isBreakpointed;
			}
		}

		public Stack<int> StepStack
		{
			get
			{
				return _stepStack;
			}
		}
		#endregion

		#region Events

		public delegate void DebuggerRunning(object sender, DebuggerRunningEventArgs e);
		public event DebuggerRunning OnDebuggerRunningChanged;

		public delegate void DebuggerBreakpointHit(object sender, DebuggerBreakpointHitEventArgs e);
		public event DebuggerBreakpointHit OnDebuggerBreakpointHit;

		public delegate void DebuggerStep(object sender, DebuggerStepEventArgs e);
		public event DebuggerStep OnDebuggerStep;

		#endregion

		public WabbitcodeDebugger(IDocumentService documentService, ISymbolService symbolService)
		{
			_disposed = false;
			_isBreakpointed = false;

			_documentService = documentService;
			_symbolService = symbolService;
		}

		// TODO: this all sucks
		public void AddBreakpoint(int lineNumber, string fileName)
		{
			WabbitcodeBreakpoint newBreakpoint = new WabbitcodeBreakpoint();

			CalcLocation value = _symbolService.ListTable.GetCalcLocation(fileName, lineNumber);
			if (value == null)
			{
				return;
			}

			newBreakpoint.Address = value.Address;
			newBreakpoint.Page = value.Page;
			newBreakpoint.IsRam = newBreakpoint.Address > 0x8000;
			if (_isAnApp && !newBreakpoint.IsRam)
			{
				newBreakpoint.Page = (byte)(_appPage - newBreakpoint.Page);
			}
			IBreakpoint wabbitBreakpoint = _debugger.SetBreakpoint(newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
			newBreakpoint.WabbitemuBreakpoint = wabbitBreakpoint;

			newBreakpoint.File = fileName;
			newBreakpoint.LineNumber = lineNumber;
			newBreakpoint.Enabled = true;

			_breakpoints.Add(newBreakpoint);
		}

		private void BreakpointHit(object sender, BreakpointEventArgs e)
		{
			IBreakpoint breakEvent = e.Breakpoint;
			ushort address = breakEvent.Address.Address;
			int page = breakEvent.Address.Page.Index;
			int relativePage = _isAnApp ? _appPage - page : page;
			WabbitcodeBreakpoint breakpoint = FindBreakpoint(address, (byte)relativePage, address >= 0x8000);
			if (breakpoint == null)
			{
				return;
			}

			// throw new Exception("Breakpoint not found!");
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

			// breakpoint.breakCondition = new List<GlobalClass.BreakCondition>();
			// GlobalClass.BreakCondition newCondition = new GlobalClass.BreakCondition();
			// newCondition.h = 5 << 16;
			// newCondition.l = 5 << 16;
			// breakpoint.breakCondition.Add(newCondition);
			if (breakpoint.BreakConditions != null)
			{
				conditionsTrue = breakpoint.BreakConditions.Aggregate(conditionsTrue, (current, condition) => current & breakpoint.EvalCondition(_debugger.CPU, condition));

				if (conditionsTrue)
				{
					DocumentLocation key = _symbolService.ListTable.GetFileLocation(relativePage, address, !breakEvent.Address.Page.IsFlash);
					if (key == null)
					{
						throw new Exception("Unable to find breakpoint");
					}

					_isBreakpointed = true;

					if (OnDebuggerBreakpointHit != null)
					{
						OnDebuggerBreakpointHit(this, new DebuggerBreakpointHitEventArgs(key));
					}
				}
				else
				{
					_isBreakpointed = false;
					_debugger.Running = true;
				}
			}
		}

		public void CancelDebug()
		{
			_isAnApp = false;

			if (_debugger == null)
			{
				return;
			}
			_debugger.Running = false;
			_debugger.Visible = false;
			_debugger = null;
		}

		private static WabbitcodeBreakpoint FindBreakpoint(ushort address, byte page, bool isRam)
		{
			return FindBreakpoint(new WabbitcodeBreakpoint(address, page, isRam));
		}

		public static WabbitcodeBreakpoint FindBreakpoint(WabbitcodeBreakpoint breakpoint)
		{
			return WabbitcodeBreakpointManager.Breakpoints.FirstOrDefault(pointToCheck => breakpoint == pointToCheck);
		}

		public static WabbitcodeBreakpoint FindBreakpoint(string file, int lineNumber)
		{
			return FindBreakpoint(new WabbitcodeBreakpoint(file, lineNumber));
		}

		public Image GetScreenImage()
		{
			return _debugger.GetScreenImage();
		}

		public void GotoAddress(ushort address)
		{
			int page = GetPageNum(address);
			DocumentLocation key = _symbolService.ListTable.GetFileLocation(address, page, address >= 0x8000);
			if (key == null)
			{
				return;
			}

			_documentService.GotoLine(key.FileName, key.LineNumber);
		}

		public bool IsBreakpointPossible(int lineNumber, string fileName)
		{
			if (_symbolService.ListTable == null)
			{
				return false;
			}
			CalcLocation value = _symbolService.ListTable.GetCalcLocation(fileName, lineNumber);
			return value != null;
		}

		public void RemoveBreakpoint(int lineNumber, string fileName)
		{
			WabbitcodeBreakpoint newBreakpoint = FindBreakpoint(fileName, lineNumber);
			if (newBreakpoint == null)
			{
				return;
			}

			if (_debugger != null)
			{
				_debugger.ClearBreakpoint(newBreakpoint.WabbitemuBreakpoint);
			}

			_breakpoints.Remove(newBreakpoint);
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
			if (_isAnApp)
			{
				page = (byte)(_appPage - page);
				_debugger.Memory.Bank[1] = _debugger.Memory.Flash[page];
			}
			else
			{
				_debugger.Memory.Bank[2] = _debugger.Memory.RAM[1];
			}
		}

		public void Step()
		{
			_isBreakpointed = false;
			//			DocumentService.HighlightCall();
			// need to clear the old breakpoint so lets save it
			ushort currentPC = _debugger.CPU.PC;
			byte oldPage = GetPageNum(currentPC);
			DocumentLocation key = _symbolService.ListTable.GetFileLocation(oldPage, currentPC, currentPC >= 0x8000);
			DocumentLocation newKey = key;
			while (newKey == null || newKey.LineNumber == key.LineNumber)
			{
				_debugger.Step();
				newKey = _symbolService.ListTable.GetFileLocation(GetPageNum(_debugger.CPU.PC), _debugger.CPU.PC, _debugger.CPU.PC >= 0x8000);
			}

			ushort address = _debugger.CPU.PC;
			byte page = GetPageNum(address);

			key = _symbolService.ListTable.GetFileLocation(page, address, address >= 0x8000);

			_isBreakpointed = true;

			if (OnDebuggerStep != null)
			{
				OnDebuggerStep(this, new DebuggerStepEventArgs(key));
			}
		}

		public void StepOut()
		{
			_isBreakpointed = false;
			// TODO: work
		}

		public void StepOver()
		{
			_isBreakpointed = false;
			_documentService.HighlightCall();
			// need to clear the old breakpoint so lets save it
			ushort currentPC = _debugger.CPU.PC;
			byte oldPage = GetPageNum(currentPC);
			DocumentLocation key = _symbolService.ListTable.GetFileLocation(oldPage, currentPC, currentPC >= 0x8000);

			// TODO: move this out into a filereader service
			string[] lines;
			StreamReader reader = null;
			try
			{
				reader = new StreamReader(key.FileName);
				lines = reader.ReadToEnd().Split('\n');
			}
			catch (Exception)
			{
				return;
			}
			finally
			{
				if (reader != null)
				{
					reader.Dispose();
				}
			}

			string line = lines[key.LineNumber - 1];
			int commentIndex = line.IndexOf(";");
			if (commentIndex != -1)
			{
				line = line.Substring(0, commentIndex);
			}

			// if the line contains a special commmand (i.e. one that will go to who knows where)
			// we just want to step over it
			string[] specialCommands = {"jp", "jr", "ret", "djnz"};
			if (specialCommands.Any(s => line.Contains(s)))
			{
				_debugger.Step();
				_isBreakpointed = true;
				currentPC = _debugger.CPU.PC;
				oldPage = GetPageNum(currentPC);
				DocumentLocation newKey = _symbolService.ListTable.GetFileLocation(oldPage, currentPC, currentPC >= 0x8000);
				// TODO: handle a null here
				if (OnDebuggerStep != null)
				{
					OnDebuggerStep(this, new DebuggerStepEventArgs(newKey));
				}
				return;
			}

			_stepOverLineNumber = key.LineNumber;
			line = lines[_stepOverLineNumber];
			// otherwise, step until we get to a known location in our codebase
			while (lines.Length > _stepOverLineNumber && (!char.IsWhiteSpace(line[0]) ||
				_symbolService.ListTable.GetCalcLocation(key.FileName, _stepOverLineNumber) == null))
			{
				_stepOverLineNumber++;
				line = lines[_stepOverLineNumber];
			}
			CalcLocation value = _symbolService.ListTable.GetCalcLocation(key.FileName, _stepOverLineNumber);
			bool isRam = value.Address >= 0x8000;
			byte page = (byte)(isRam ? value.Page : AppPage - value.Page);
			_stepOverBreakpoint = _debugger.SetBreakpoint(isRam, page, value.Address);
			_debugger.OnBreakpoint += StepOverBreakpointEvent;

			_debugger.Step();
			_debugger.Running = true;
		}

		private void StepOverBreakpointEvent(IWabbitemu sender, EventArgs e)
		{
			_debugger.ClearBreakpoint(_stepOverBreakpoint);
			int page = _stepOverBreakpoint.Address.Page.IsFlash
				? AppPage - _stepOverBreakpoint.Address.Page.Index
				: _stepOverBreakpoint.Address.Page.Index;
			DocumentLocation key = _symbolService.ListTable.GetFileLocation(page, 
				_stepOverBreakpoint.Address.Address,
				!_stepOverBreakpoint.Address.Page.IsFlash);

			_isBreakpointed = true;
			if (OnDebuggerStep != null)
			{
				OnDebuggerStep(this, new DebuggerStepEventArgs(key));
			}
			_debugger.OnBreakpoint -= StepOverBreakpointEvent;
		}

		internal byte GetPageNum(ushort address)
		{
			int page = _appPage - _debugger.Memory.Bank[address >> 14].Index;
			return (byte)page;
		}

		internal void InitDebugger(string outputFile)
		{
			_debugger = new WabbitemuDebugger();
			_debugger.LoadFile(outputFile);
			_debugger.Visible = true;
			_debugger.OnBreakpoint += BreakpointHit;
		}

		internal void Pause()
		{
			_isBreakpointed = false;

			ushort currentPC = _debugger.CPU.PC;
			DocumentLocation key = _symbolService.ListTable.GetFileLocation(currentPC, GetPageNum(currentPC), currentPC >= 0x8000);
			if (key == null)
			{
				throw new Exception("Unable to pause here");
			}

			_debugger.Running = false;

			if (OnDebuggerRunningChanged != null)
			{
				OnDebuggerRunningChanged(this, new DebuggerRunningEventArgs(key, false));
			}
		}

		internal byte ReadByte(ushort address)
		{
			return _debugger.Memory.ReadByte(address);
		}

		public ushort ReadShort(ushort address)
		{
			return _debugger.Memory.ReadWord(address);
		}

		internal void Run()
		{
			_isBreakpointed = false;
			if (_debugger == null)
			{
				return;
			}

			_debugger.Step();
			_debugger.Running = true;

			if (OnDebuggerRunningChanged != null)
			{
				OnDebuggerRunningChanged(this, new DebuggerRunningEventArgs(null, true));
			}
		}

		internal void SetBreakpoint(WabbitcodeBreakpoint newBreakpoint)
		{
			if (_debugger != null)
			{
				CalcLocation location = _symbolService.ListTable.GetCalcLocation(newBreakpoint.File, newBreakpoint.LineNumber);
				newBreakpoint.Page = location.Page;
				newBreakpoint.Address = location.Address;
				newBreakpoint.IsRam = location.IsRam;
				newBreakpoint.WabbitemuBreakpoint = _debugger.SetBreakpoint(newBreakpoint.IsRam,
					(byte) (AppPage - newBreakpoint.Page), newBreakpoint.Address);
			}
		}

		internal void ClearBreakpoint(WabbitcodeBreakpoint newBreakpoint)
		{
			if (_debugger != null)
			{
				_debugger.ClearBreakpoint(newBreakpoint.WabbitemuBreakpoint);
			}
		}

		internal void SimulateKeyPress(Keys key)
		{
			_debugger.Keypad.PressVirtKey((int)key);
			Thread.Sleep(50);
			_debugger.Keypad.ReleaseVirtKey((int)key);
		}

		internal void VerifyApp(string createdName)
		{
			if (Apps.Length == 0)
			{
				throw new DebuggingException("Application not found on calc");
			}

			char[] buffer = new char[8];
			StreamReader appReader = new StreamReader(createdName);
			for (int i = 0; i < 17; i++)
			{
				appReader.Read();
			}

			appReader.ReadBlock(buffer, 0, 8);

			appReader.Dispose();
			string appName = new string(buffer);
			TIApplication? app = Apps.Cast<TIApplication>().SingleOrDefault(a => a.Name == appName);
			if (app == null)
			{
				throw new DebuggingException("Application not found on calc");
			}

			AppPage = (byte)app.Value.Page.Index;
		}

		internal string GetOutputFileDetails(IProject project)
		{
			if (string.IsNullOrEmpty(project.BuildSystem.ProjectOutput))
			{
				throw new DebuggingException("No project outputs detected");
			}

			string createdName = project.BuildSystem.ProjectOutput;
			if (!Path.IsPathRooted(createdName))
			{
				createdName = FileOperations.NormalizePath(Path.Combine(project.ProjectDirectory, createdName));
			}

			if (string.IsNullOrEmpty(project.BuildSystem.ListOutput))
			{
				throw new DebuggingException("Missing list file");
			}

			string listName = project.BuildSystem.ListOutput;
			if (string.IsNullOrEmpty(project.BuildSystem.LabelOutput))
			{
				throw new DebuggingException("Missing label file");
			}

			string symName = project.BuildSystem.LabelOutput;

			StreamReader listReader = null;
			try
			{
				listReader = new StreamReader(listName);
				listReader.ReadToEnd();
			}
			catch (Exception)
			{
				throw new DebuggingException("Error reading list file");
			}
			finally
			{
				if (listReader != null)
				{
					listReader.Dispose();
				}
			}

			StreamReader symReader = null;
			try
			{
				symReader = new StreamReader(symName);
				symReader.ReadToEnd();

			}
			catch (Exception)
			{
				throw new DebuggingException("Error reading label file");
			}
			finally
			{
				if (symReader != null)
				{
					symReader.Dispose();
				}
			}

			_isAnApp = createdName.EndsWith(".8xk");
			return createdName;
		}

		//private void StartWithoutDebuggingAssemblerFinished(object sender, AssemblyFinishEventArgs e)
		//{
		//	if (!e.AssemblySucceeded)
		//	{
		//		DockingService.ShowError("Assembly failed");
		//	}

		//	if (ProjectService.Project.ProjectOutputs.Count < 1)
		//	{
		//		DockingService.ShowError("No project outputs detected");
		//		return;
		//	}

		//	string createdName = ProjectService.Project.ProjectOutputs[0];
		//	Resources.GetResource("Wabbitemu.exe", FileLocations.WabbitemuFile);
		//	Process wabbit = Process.GetProcesses().FirstOrDefault(potential => potential.ProcessName.Contains("wabbitemu", StringComparison.OrdinalIgnoreCase));

		//	if (wabbit == null)
		//	{
		//		wabbit = new Process
		//		{
		//			StartInfo =
		//			{
		//				Arguments = "\"" + createdName + "\"",
		//				FileName = FileLocations.WabbitemuFile,
		//			},
		//			EnableRaisingEvents = true
		//		};
		//		wabbit.Start();
		//	}
		//}

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
					_debugger.Dispose();
				}
			}
			_disposed = true;
		}
	}
}