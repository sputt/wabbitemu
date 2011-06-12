using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Threading;
using System.Windows.Forms;
using Microsoft.Win32;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using System.Linq;
#if NEW_DEBUGGING
using WabbitemuLib;
using Revsoft.Wabbitcode.Services.Debugger;
#endif
using System.Text;
using System.Diagnostics;

namespace Revsoft.Wabbitcode.Services
{
	public static class DebuggerService
	{
		private static CWabbitemu debugger;
        public static CWabbitemu Debugger
		{
			get { return debugger; }
			set { debugger = value; }
		}

        private static List<WabbitcodeBreakpoint> breakpoints = new List<WabbitcodeBreakpoint>();
		private static Stack<int> stepStack;
		private static bool isDebugging;
		private static byte appPage;
		private static bool isAnApp;
		private static bool isBreakpointed;

		public static Stack<int> StepStack
		{
			get { return stepStack; }
		}

		public static bool IsAnApp
		{
			get { return isAnApp; }
			set { isAnApp = value; }
		}

		public static byte AppPage
		{
			get { return appPage; }
			set { appPage = value; }
		}

		public static bool IsDebugging
		{
			get { return isDebugging; }
			set { isDebugging = value; }
		}
		public static bool IsBreakpointed
		{
			get { return isBreakpointed; }
			set { isBreakpointed = value; }
		}
		public static List<WabbitcodeBreakpoint> Breakpoints
		{
			get { return breakpoints; }
			set { breakpoints = value; }
		}

		public static WabbitcodeBreakpoint FindBreakpoint(ushort address, byte page, bool isRam)
		{
			return FindBreakpoint(new WabbitcodeBreakpoint(address, page, isRam));
		}

		public static WabbitcodeBreakpoint FindBreakpoint(WabbitcodeBreakpoint breakpoint)
		{
			foreach (WabbitcodeBreakpoint pointToCheck in breakpoints)
			{
				if (breakpoint == pointToCheck)
					return pointToCheck;
			}
			return null;
		}

		public static WabbitcodeBreakpoint FindBreakpoint(string file, int lineNumber)
		{
			return FindBreakpoint(new WabbitcodeBreakpoint(file, lineNumber));
		}

		public static bool IsBreakpointPossible(int lineNumber, string fileName)
		{
			return debugTable == null || debugTable.ContainsKey(new ListFileKey(fileName.ToLower(), (lineNumber + 1)));
		}

		public static void RemoveBreakpoint(int lineNumber, string fileName)
		{
			WabbitcodeBreakpoint newBreakpoint = FindBreakpoint(fileName, lineNumber);
			if (newBreakpoint == null)
				return;
			if (debugger != null)
			{
#if NEW_DEBUGGING
                //debugger.ClearBreakpoint(new CPage(newBreakpoint.Page, newBreakpoint.IsRam), newBreakpoint.Address);
#else
				debugger.clearBreakpoint(newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
#endif
			}
			breakpoints.Remove(newBreakpoint);
		}
		public static void AddBreakpoint(int lineNumber, string fileName)
		{
			WabbitcodeBreakpoint newBreakpoint = new WabbitcodeBreakpoint();
			if (debugTable != null)
			{
				ListFileValue value;
				ListFileKey key =  new ListFileKey(fileName.ToLower(), lineNumber + 1);
				debugTable.TryGetValue(key, out value);
				if (value == null)
					return;
				newBreakpoint.Address = value.Address;
				newBreakpoint.Page = value.Page;
                newBreakpoint.IsRam = newBreakpoint.Address > 0x8000;
				if (isAnApp && !newBreakpoint.IsRam)
					newBreakpoint.Page = (byte)(appPage - newBreakpoint.Page);
#if NEW_DEBUGGING
                debugger.SetBreakpoint(new CPage(newBreakpoint.Page, newBreakpoint.IsRam), newBreakpoint.Address);
#else
				debugger.setBreakpoint(newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
#endif
			}
			else
			{
				newBreakpoint.IsRam = false;
				newBreakpoint.Address = 0;
				newBreakpoint.Page = 0;
			}
			newBreakpoint.file = fileName;
			newBreakpoint.lineNumber = lineNumber;
			newBreakpoint.Enabled = true;
			breakpoints.Add(newBreakpoint);
		}

		private static Dictionary<ListFileKey, ListFileValue> debugTable;
		private static void ParseListFile(string listFileContents, string assembledFile, string projectPath)
		{
			string currentFile = assembledFile;
			string[] lines = listFileContents.Split('\n');
			int currentLine = 0;
			foreach (string line in lines)
			{
				if (line == "" || line == "\r")
					continue;
				if (line.Contains("Listing for file"))
				{
					currentFile = Path.Combine(projectPath, line.Substring(line.IndexOf('\"') + 1,
									line.LastIndexOf('\"') - line.IndexOf('\"') - 1)).ToLower();
				}
				if (line.Substring(0, 5) != "     " && line.Substring(13, 12) != "            " &&
					line.Substring(13, 12) != " -  -  -  - " && !line.Contains("Listing for"))
				{
					string stringLineNumber = line.Substring(0, 5).Trim();
					int lineNumber = Convert.ToInt32(stringLineNumber);
					currentLine = lineNumber;
					string temp = line.Substring(6, 7);
					ushort address = ushort.Parse(temp.Substring(3, 4), NumberStyles.HexNumber);
					byte page = byte.Parse(temp.Substring(0, 2), NumberStyles.HexNumber);
					//correction for ram pages
					if (page == 0 && address >= 0x8000 && address < 0xC000)
						page = 1;
					ListFileKey key = new ListFileKey(currentFile, lineNumber);
					ListFileValue value = new ListFileValue(address, page);
					
					//shouldnt be necessary
					if (!debugTable.ContainsKey(key))
						debugTable.Add(key, value);
				}
			}
		}

		public static void BreakpointHit()
		{
#if NEW_DEBUGGING
            ushort address = debugger.CPU.PC;
#else
			ushort address = debugger.getState().PC;
#endif
			byte page = GetPageNum(address);
            byte breakpointPage = isAnApp ? (byte)(appPage - page) : page;
			WabbitcodeBreakpoint breakpoint = FindBreakpoint(address, breakpointPage, address > 0x8000);
			if (breakpoint == null)
				return;
				//throw new Exception("Breakpoint not found!");
			breakpoint.numberOfTimesHit++;
			bool conditionsTrue = breakpoint.Enabled;
			switch (breakpoint.hitCountCondition)
			{
				case HitCountEnum.BreakEqualTo:
					if (breakpoint.numberOfTimesHit != breakpoint.hitCountConditionNumber)
						conditionsTrue &= false;
					break;
				case HitCountEnum.BreakGreaterThanEqualTo:
					if (breakpoint.numberOfTimesHit < breakpoint.hitCountConditionNumber)
						conditionsTrue &= false;
					break;
				case HitCountEnum.BreakMultipleOf:
					if (breakpoint.numberOfTimesHit % breakpoint.hitCountConditionNumber != 0)
						conditionsTrue &= false;
					break;
			}
			//breakpoint.breakCondition = new List<GlobalClass.BreakCondition>();
			//GlobalClass.BreakCondition newCondition = new GlobalClass.BreakCondition();
			//newCondition.h = 5 << 16;
			//newCondition.l = 5 << 16;
			//breakpoint.breakCondition.Add(newCondition);
			if (breakpoint.breakCondition != null)
				foreach (BreakCondition condition in breakpoint.breakCondition)
					conditionsTrue &= breakpoint.EvalCondition(condition);
			if (conditionsTrue)
			{
				ListFileValue value = new ListFileValue(address, page);
				ListFileKey key;
				debugTable.TryGetKey(value, out key);;
                if (key == null)
                {
                    DockingService.ShowError("Unable to find breakpoint");
                    return;
                }
				DocumentService.GotoLine(key.FileName, key.LineNumber);
				DocumentService.HighlightDebugLine(key.LineNumber);
				isBreakpointed = true;

				//this reinitiates all the good stuff
				IntPtr calculatorHandle = DockingService.MainForm.Handle;
				//switch to back to us
				NativeMethods.SetForegroundWindow(calculatorHandle);
				DockingService.MainForm.UpdateDebugStuff();
				UpdateStack();
				DockingService.MainForm.UpdateTrackPanel();
				DockingService.MainForm.UpdateDebugPanel();
			}
			else
			{
				isBreakpointed = false;
#if NEW_DEBUGGING
                debugger.Running = true;
#else
				debugger.run();
#endif
			}
		}

		private static Thread stepThread;
		public static void StepOut()
		{
            isBreakpointed = false;
			stepThread = new Thread(DebuggerService.DoStep);
			stepThread.Start(StepType.StepOut);
            if (stepStack.Count > 0)
                stepStack.Pop();
		}

		public static void StepOver()
		{
            isBreakpointed = false;
			stepThread = new Thread(DebuggerService.DoStep);
			stepThread.Start(StepType.StepOver);
		}

		public static void Step()
		{
            isBreakpointed = false;
			DocumentService.HighlightCall();
			stepThread = new Thread(DebuggerService.DoStep);
			stepThread.Start(StepType.Step);
		}

		public enum StepType
		{
			Step,
			StepOver,
			StepOut
		}

		private delegate void DoneStep(ListFileKey newKey);
		public static void DoStep(object steptype)
		{
			StepType stepType = (StepType)steptype;
			/*foreach (newEditor child in GlobalClass.mainForm.MdiChildren)
				if (child.DocumentChanged)
				{
					if (!isAnApp)
						MessageBox.Show("Edit and continue is not available for programs.\nRestart your debugging session.", "Unable to continue", MessageBoxButtons.OK, MessageBoxIcon.Error);
					if (!editAndContinue())
						return;
					string pageandpc = debugTableReverse[child.editorBox.FileName.ToLower() + ":" + child.editorBox.Document.GetLineNumberForOffset(highlight.Offset) + 1].ToString();
					ushort address = Convert.ToUInt16(pageandpc.Substring(3, 4), 16);
					//byte page = Convert.ToByte(pageandpc.Substring(4, 2), 16);
					state = debugger.getState();
					state.PC = address;
					debugger.setState(state);
					// CWabbitemu.MEMSTATE mem = debugger.getMemState();
				}*/
			//need to clear the old breakpoint so lets save it
#if NEW_DEBUGGING
            ushort currentPC = debugger.CPU.PC;
#else
			ushort currentPC = debugger.getState().PC;
#endif
			byte oldPage = GetPageNum(currentPC);
			ListFileValue value = new ListFileValue(currentPC, oldPage);
			ListFileKey key;
            debugTable.TryGetKey(value, out key);
			ListFileKey newKey = key;
			while (newKey.LineNumber == key.LineNumber)
			{
				switch (stepType)
				{
#if NEW_DEBUGGING
                    case StepType.Step:
                        debugger.Step();
                        break;
                    case StepType.StepOver:
                        debugger.StepOver();
                        break;
                    case StepType.StepOut:
                        //debugger.StepOut();
                        break;
#else
					case StepType.Step:
						debugger.step();
						break;
					case StepType.StepOver:
						debugger.stepOver();
						break;
					case StepType.StepOut:
						debugger.stepOut();
						break;
#endif
				}
#if NEW_DEBUGGING
                ushort address = debugger.CPU.PC;
#else
				ushort address = debugger.getState().PC;
#endif
				
				byte page = GetPageNum(address);
				value = new ListFileValue(address, page);
				if (debugTable == null)
					return;
				if (debugTable.ContainsValue(value))
				{
					if (page != oldPage && stepType == StepType.StepOver)
						continue;
					//need to get the new info
					debugTable.TryGetKey(value, out newKey);
				}
			}
            isBreakpointed = true;
			DoneStep doneStep = new DoneStep(DockingService.MainForm.DoneStep);
			DockingService.MainForm.Invoke(doneStep, new object[] { newKey });
		}

		internal static byte GetPageNum(ushort address)
		{
			byte page = 0xFF;
#if NEW_DEBUGGING
            CWabbitemu.MEMSTATE memstate = new CWabbitemu.MEMSTATE();// = GetMemState(currentSlot);
#else
			CWabbitemu.MEMSTATE memstate = debugger.getMemState();
#endif
			if (address < 0x4000)
				page = memstate.page0;
            if (address >= 0x4000 && address < 0x8000) // && memstate.ram1 == 0)
                if (isAnApp)
                    page = (byte)(appPage - memstate.page1);
                else
                    page = memstate.page1;
			if (address >= 0x8000 && address < 0xC000) // && memstate.ram2 == 0)
				page = memstate.page2;
			if (address >= 0xC000)
				page = memstate.page3;
			return page;
		}

		public static void GotoAddress(ushort address)
        {
			ListFileValue value = new ListFileValue(address, GetPageNum(address));
			if (!debugTable.ContainsValue(value))
                return;
			ListFileKey key;
			debugTable.TryGetKey(value, out key);
            DocumentService.GotoLine(key.FileName, key.LineNumber);
        }

		private static ushort oldSP = 0xFFFF;
		public static void UpdateStack()
		{
#if NEW_DEBUGGING
            int currentSP = debugger.CPU.SP;
#else
			int currentSP = debugger.getState().SP;
#endif
			//if someone has changed sp we dont want a really big callstack
			if (currentSP < 0xFE66)
				return;
			int bytesToRead = 0xFFFF - currentSP;
			while (oldSP != currentSP - 2)
			{
				if (oldSP > currentSP - 2)
				{
#if NEW_DEBUGGING
                    DockingService.CallStack.AddStackData(oldSP, (int)debugger.Read(oldSP) + (int)debugger.Read((ushort)(oldSP + 1)) * 256);
#else
					DockingService.CallStack.AddStackData(oldSP, debugger.readMem(oldSP) + debugger.readMem((ushort)(oldSP + 1)) * 256);
#endif
					oldSP-= 2;
				}
				else
				{
					DockingService.CallStack.RemoveLastRow();
					oldSP += 2;
				}
			}
		}

		private static SymbolTableClass symTable;
		public static SymbolTableClass SymbolTable
		{
			get { return symTable; }
		}
		
		private delegate void AssembleProjectDelegate();
		private delegate void StartDebugDelegate();
		private delegate void UpdateBreaksDelegate();
		internal static void StartDebug()
		{
			Thread debugThread = new Thread(InitDebug);
			debugThread.Start();
			DockingService.MainForm.StartDebug();
		}

		private static void InitDebug()
		{
			isDebugging = true;
			string listName, symName, fileName = "", startAddress, createdName;
			bool error = true;
			if (!ProjectService.IsInternal)
			{
				int outputType = ProjectService.Project.GetOutputType();
				startAddress = outputType == 5 ? "4080" : "9D95";
				bool configFound = false;
				foreach (Services.Project.BuildConfig config in ProjectService.BuildConfigs)
					if (config.Name.ToLower() == "debug")
						configFound = true;
				if (configFound)
				{
                    ThreadPool.QueueUserWorkItem(AssemblerService.AssembleProject);
                    //wait till we notice that it is building
                    while (!AssemblerService.IsBuildingProject)
                        Application.DoEvents();
                    //then wait till it builds
					while (AssemblerService.IsBuildingProject)
						Application.DoEvents();
                    if (ProjectService.Project.ProjectOutputs.Count < 1)
                    {
                        DockingService.ShowError("No project outputs detected");
                        CancelDebug();
                        return;
                    }
					createdName = ProjectService.Project.ProjectOutputs[0];
                    fileName = ProjectService.Project.BuildSystem.MainFile;
					if (!Path.IsPathRooted(createdName))
						createdName = FileOperations.NormalizePath(Path.Combine(ProjectService.ProjectDirectory, createdName));
                    if (ProjectService.Project.ListOutputs.Count < 1)
                    {
                        DockingService.ShowError("No List file was found");
                        CancelDebug();
                        return;
                    }
                    listName = ProjectService.Project.ListOutputs[0];
                    if (ProjectService.Project.LabelOutputs.Count < 1)
                    {
                        DockingService.ShowError("No label file was found");
                        CancelDebug();
                        return;
                    }
                    symName = ProjectService.Project.LabelOutputs[0];
					var errorList = from err in AssemblerService.ErrorsInFiles where !err.isWarning select err;
					error = errorList.Count() == 0;
				}
				else
				{
					DockingService.ShowError("No build config named Debug was found. Make sure you have setup a debug build config");
                    CancelDebug();
					return;
				}
			}
			else
			{
				fileName = DocumentService.ActiveFileName;
				if (string.IsNullOrEmpty(fileName))
				{
					DockingService.ShowError("No files open, cannot debug");
					CancelDebug();
					return;
				}
				else if (DocumentService.ActiveDocument.DocumentChanged)
				{
					if (MessageBox.Show("You must save before you debug. Would you like to save this file?", "Save", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
					{
						DocumentService.SaveDocument();
						fileName = DocumentService.ActiveFileName;
					}
					else
					{
						CancelDebug();
						return;
					}
				}

				error &= AssemblerService.AssembleFile(fileName, Path.ChangeExtension(fileName, AssemblerService.GetExtension(Settings.Default.outputFile)), false);
				//if (error)
				//    MessageBox.Show("Problem creating 8xp file");
				listName = Path.ChangeExtension(fileName, ".lst");
				symName = Path.ChangeExtension(fileName, ".lab");
				switch (Settings.Default.outputFile)
				{
					case 4:
						startAddress = "9D95";
						createdName = Path.ChangeExtension(fileName, "8xp");
						break;
					case 5:
						startAddress = "4080";
						createdName = Path.ChangeExtension(fileName, "8xk");
						break;
					default:
						MessageBox.Show("You cannot debug a non 83/84 Plus file");
						CancelDebug();
						return;
				}
			}
			if (!isDebugging || !error)
			{
				if (MessageBox.Show("There were errors assembling. Would you like to continue and try to debug?",
									"Continue", MessageBoxButtons.YesNo, MessageBoxIcon.Error) == DialogResult.No)
				{
					CancelDebug();
					return;
				}
			}
#if NEW_DEBUGGING
			debugger = new CWabbitemu(createdName);
#else
			debugger = new CWabbitemu(createdName);
#endif
			stepStack = new Stack<int>();
            StreamReader reader = null;
            string listFileText, symFileText = "";
			try
			{
				reader = new StreamReader(listName);
				listFileText = reader.ReadToEnd();
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error reading list file:", ex);
				CancelDebug();
				return;
			}
			finally
			{
				if (reader != null)
					reader.Close();
			}
			try
			{
				reader = new StreamReader(symName);
				symFileText = reader.ReadToEnd();

			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error reading symbol file:", ex);
			}
			finally
			{
				if (reader != null)
				{
					reader.Dispose();
					reader.Close();
				}
			}

			debugTable = new Dictionary<ListFileKey, ListFileValue>();
			ParseListFile(listFileText, fileName, Path.GetDirectoryName(fileName));
			symTable = new SymbolTableClass();
			symTable.ParseSymFile(symFileText);
			if (startAddress == "4080")
			{
				isAnApp = true;
#if NEW_DEBUGGING
                if (debugger.Apps.Length == 0)
                {
                    CancelDebug();
                    return;
                }
                while (((TIApplication)debugger.Apps.GetValue(0))/*[0]*/.PageCount == 0)
                    Thread.Sleep(500);
#else
                CWabbitemu.AppEntry[] appList = new CWabbitemu.AppEntry[20];
				while (appList[0].page_count == 0)
				{
					appList = debugger.getAppList();
					if (appList == null)
					{
						CancelDebug();
						return;
					}
                    Thread.Sleep(500);
				}
#endif
				int counter = 0;
                char[] buffer = new char[8];
                StreamReader appReader = new StreamReader(createdName);
                for (int i = 0; i < 17; i++)
                    appReader.Read();
                appReader.ReadBlock(buffer, 0, 8);

				appReader.Dispose();
                appReader.Close();
                string appName = new string(buffer).Trim();
#if NEW_DEBUGGING
                foreach (TIApplication app in debugger.Apps)
                {
                    if (app.Name.Trim() == appName)
                        break;
                    counter++;
                }
                if (counter > debugger.Apps.Length)
                {
                    if (MessageBox.Show("Unable to find the app, would you like to try and continue and debug?", "Missing App", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation) != DialogResult.Yes)
                    {
                        CancelDebug();
                        return;
                    }
                    counter = 0;
                }
                appPage = (byte) ((TIApplication)debugger.Apps.GetValue(counter))/*[counter]*/.Page.Index;
#else
				foreach (CWabbitemu.AppEntry app in appList)
				{
                    StringBuilder sb = new StringBuilder();
                    sb.Append(app.name1);
                    sb.Append(app.name2);
                    sb.Append(app.name3);
                    sb.Append(app.name4);
                    sb.Append(app.name5);
                    sb.Append(app.name6);
                    sb.Append(app.name7);
                    sb.Append(app.name8);
					if (sb.ToString().Trim().ToLower() == appName.ToLower())
						break;
					counter++;
				}
				if (counter == appList.Length)
				{
                    if (MessageBox.Show("Unable to find the app, would you like to try and continue and debug?", "Missing App", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation) != DialogResult.Yes)
                    {
                        CancelDebug();
                        return;
                    }
                    counter = 0;
				}
				appPage = (byte)appList[counter].page;
				//apps key
				debugger.sendKeyPress(Keys.B);
				Thread.Sleep(50);
				debugger.releaseKeyPress(Keys.B);
				while (counter != -1)
				{
					debugger.sendKeyPress(Keys.Down);
					Thread.Sleep(50);
					debugger.releaseKeyPress(Keys.Down);
					counter--;
				}
				debugger.sendKeyPress(Keys.Enter);
				Thread.Sleep(50);
				debugger.releaseKeyPress(Keys.Enter);
#endif
			}

			UpdateBreaksDelegate updateBreaks = DockingService.MainForm.UpdateBreakpoints;
			DockingService.MainForm.Invoke(updateBreaks);
			UpdateBreaksDelegate updateDebugStuff = DockingService.MainForm.UpdateDebugStuff;
			DockingService.MainForm.Invoke(updateDebugStuff);
			//staticLabelsParser.DoWork += new DoWorkEventHandler(staticLabelsParser_DoWork);
			//if (!staticLabelsParser.IsBusy && !DockingService.MainForm.IsDisposed && !DockingService.MainForm.Disposing)
			//	staticLabelsParser.RunWorkerAsync();
		}

        private delegate void AddMarkersDelegate(List<TextMarker> markers);
        private static void staticLabelsParser_DoWork(object sender, DoWorkEventArgs e)
        {
            string text;
            if (e.Argument == null)
            {
                foreach (newEditor child in DockingService.Documents)
                {
                    text = child.EditorText;
                    List<TextMarker> markers = GetStaticLabels(text);
                    AddMarkersDelegate markersDelegate = child.AddMarkers;
                    DockingService.MainForm.Invoke(markersDelegate, markers);
                }
            }
            else
            {
                newEditor child = ((newEditor)e.Argument);
                text = child.EditorText;
                List<TextMarker> markers = GetStaticLabels(text);
                child.AddMarkers(markers);
            }
        }

        private static List<TextMarker> GetStaticLabels(string editorText)
        {
            int counter = 0, newCounter, textLength = editorText.Length;
            string possibleReference;
            List<TextMarker> markersToAdd = new List<TextMarker>();
            while (counter < textLength)
            {
                if (editorText[counter] == ';')
                    while (editorText[counter] != '\n')
                        counter++;
                newCounter = Parser.ParserService.GetWord(editorText, counter);
                if (newCounter == -1)
                {
                    counter++;
                    continue;
                }
                possibleReference = editorText.Substring(counter, newCounter - counter);
                if (!Settings.Default.caseSensitive)
                    possibleReference = possibleReference.ToUpper();
                if (!string.IsNullOrEmpty(possibleReference) && DebuggerService.SymbolTable.StaticLabels.Contains(possibleReference))
                {
                    TextMarker marker = new TextMarker(counter, newCounter - counter, TextMarkerType.Invisible) { Tag = "Static Label", ToolTip = DebuggerService.SymbolTable.StaticLabels[possibleReference].ToString() };
                    markersToAdd.Add(marker);
                }
                counter += possibleReference.Length + 1;
                while (counter < textLength && char.IsWhiteSpace(editorText[counter]))
                    counter++;
            }
            return markersToAdd;
        }

		static BackgroundWorker staticLabelsParser = new BackgroundWorker();
		public static void CancelDebug()
		{
			
			isDebugging = false;
			isAnApp = false;
            oldSP = 0xFFFF;
            DockingService.CallStack.Clear();
#if NEW_DEBUGGING
			if (debugger != null)
				debugger.Running = false;
#else
#if !DEBUG
            try
            {
#endif
                if (debugger != null)
                    debugger.Close();
                debugger = null;
#if !DEBUG
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error in closing the debugger", ex);
            }
#endif
#endif
			DockingService.MainForm.Invoke(new CancelDebugDelegate(DockingService.MainForm.CancelDebug));
			if (debugTable != null)
				debugTable.Clear();
			debugTable = null;
		}
		private delegate void CancelDebugDelegate();

		internal static ListFileValue GetListValue(string file, int lineNumber)
		{
			if (debugTable == null || file == null)
				return null;
			ListFileValue value;
			debugTable.TryGetValue(new ListFileKey(file.ToLower(), lineNumber), out value);
			return value;
		}

        internal static ListFileKey GetListKey(ushort address, byte page)
        {
            if (debugTable == null)
                return null;
            ListFileKey key;
            debugTable.TryGetKey(new ListFileValue(address, page), out key);
            return key;
        }

		public static void SetPCToSelect(string fileName, int lineNumber)
		{
			ListFileKey key = new ListFileKey(fileName.ToLower(), lineNumber + 1);
			if (!debugTable.ContainsKey(key))
			{
				MessageBox.Show("Unable to set statement here!");
				return;
			}
			ListFileValue value;
			debugTable.TryGetValue(key, out value);
#if NEW_DEBUGGING
            IZ80 state = debugger.CPU;
            CWabbitemu.MEMSTATE memState = new CWabbitemu.MEMSTATE();// GetMemState(currentSlot);
#else
			CWabbitemu.Z80_State state = debugger.getState();
			CWabbitemu.MEMSTATE memState = debugger.getMemState();
#endif
			state.PC = value.Address;
			byte page = value.Page;
			if (isAnApp)
			{
				page = (byte)(appPage - page);
				memState.page1 = page;
				memState.ram1 = 0;
			}
			else
			{
				memState.page2 = 1;
				memState.ram2 = 1;
			}
			DocumentService.RemoveDebugHighlight();
			DocumentService.HighlightDebugLine(lineNumber + 1);
#if NEW_DEBUGGING
            //debugger.CPU = state;
            //SetMemState(currentSlot, memState);
#else
			debugger.setMemState(memState);
			debugger.setState(state);
#endif
			DockingService.MainForm.UpdateDebugPanel();
		}

		private static bool showToolbar;
		public static bool ShowToolbar {
			get { return showToolbar; }
			set { showToolbar = value; }
		}

		internal static void Pause()
		{
            isBreakpointed = false;
			IntPtr calculatorHandle = DockingService.MainForm.Handle;
			//switch to back to us
			NativeMethods.SetForegroundWindow(calculatorHandle);
			DockingService.MainForm.UpdateDebugStuff();
			DockingService.MainForm.UpdateTrackPanel();
			DockingService.MainForm.UpdateDebugPanel();
#if NEW_DEBUGGING
            ushort currentPC = debugger.CPU.PC;
#else
			ushort currentPC = debugger.getState().PC;
#endif
            ListFileValue value = new ListFileValue(currentPC, GetPageNum(currentPC));
            if (!debugTable.ContainsValue(value))
			{
				MessageBox.Show("Unable to pause here");
				return;
			}
			ListFileKey key;
			debugTable.TryGetKey(value, out key);
			DocumentService.GotoLine(key.FileName, key.LineNumber);
			DocumentService.HighlightDebugLine(key.LineNumber);
#if NEW_DEBUGGING
            debugger.Running = false;
#else
			debugger.pause();
#endif
		}

		internal static void Run()
		{
			isBreakpointed = false;
			IntPtr calculatorHandle = NativeMethods.FindWindow("z80", "Wabbitemu");
			if ((int) calculatorHandle == 0)
			{
				StartDebug();
				return;
			}
			//switch to the emulator
			NativeMethods.SetForegroundWindow(calculatorHandle);
#if NEW_DEBUGGING
            debugger.Running = true;
#else
			DebuggerService.Debugger.run();
#endif
			if (DockingService.ActiveDocument != null)
			{
				DockingService.MainForm.UpdateDebugStuff();
				DocumentService.RemoveDebugHighlight();
				DockingService.ActiveDocument.Refresh();
			}
		}

        internal static void SetBreakpoint(WabbitcodeBreakpoint newBreakpoint)
        {
#if NEW_DEBUGGING
            debugger.SetBreakpoint(new CPage(newBreakpoint.Page, newBreakpoint.IsRam), newBreakpoint.Address);
#else
            debugger.setBreakpoint(newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
#endif
        }

		internal static void StartWithoutDebug()
		{
			if (ProjectService.CurrentBuildConfig == null)
			{
				DockingService.ShowError("No config setup");
				return;
			}
			ThreadPool.QueueUserWorkItem(AssemblerService.AssembleProject);
			//wait till we notice that it is building
			while (!AssemblerService.IsBuildingProject)
				Application.DoEvents();
			//then wait till it builds
			while (AssemblerService.IsBuildingProject)
				Application.DoEvents();
			if (ProjectService.Project.ProjectOutputs.Count < 1)
			{
				DockingService.ShowError("No project outputs detected");
				return;
			}
			string createdName = ProjectService.Project.ProjectOutputs[0];
			Classes.Resources.GetResource("Wabbitemu.exe", FileLocations.WabbitemuFile);
			Process wabbit = null;
			foreach (Process potential in Process.GetProcesses())
			{
				if (!potential.ProcessName.ToLower().Contains("wabbitemu"))
					continue;
				wabbit = potential;
				break;
			}
			if (wabbit == null)
			{
				wabbit = new Process
				{
					StartInfo =
					{
						Arguments = "\"" + createdName + "\"",
						FileName = FileLocations.WabbitemuFile,
					},
					EnableRaisingEvents = true
				};
				wabbit.Start();
			}
		}
	}

	public class ListFileKey : IEquatable<ListFileKey>
	{
		public string FileName { get; private set; }
		public int LineNumber { get; private set; }
		public ListFileKey(string file, int lineNumber)
		{
			FileName = file;
			LineNumber = lineNumber;
		}

        public override string ToString()
        {
            return "File: " + FileName + " Line: " + LineNumber;
        }

		public override int GetHashCode()
		{
			return FileName.Length + LineNumber;
		}

		public override bool Equals(object obj)
		{
			if (obj.GetType() != typeof(ListFileKey))
				return false;
			ListFileKey key = obj as ListFileKey;
			return key.FileName == FileName && key.LineNumber == LineNumber;
		}

		public bool Equals(ListFileKey key)
		{
			return key.FileName == FileName && key.LineNumber == LineNumber;
		}
	}

	public class ListFileValue : IEquatable<ListFileValue>
	{
		public ushort Address { get; private set; }
		public byte Page { get; private set; }
		public ListFileValue(ushort address, byte page)
		{
			Address = address;
			Page = page;
		}

        public override string ToString()
        {
            return "Page: " + Page + " Address: " + Address.ToString("X");
        }

		public override int GetHashCode()
		{
			return Page + Address;
		}

		public override bool Equals(object obj)
		{
			if (obj.GetType() != typeof(ListFileValue))
				return false;
			ListFileValue value = obj as ListFileValue;
			return value.Page == Page && value.Address == Address;
		}

		public bool Equals(ListFileValue value)
		{
			return value.Page == Page && value.Address == Address;
		}
	}
}
