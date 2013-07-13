using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Project;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;
using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services
{
    public class WabbitcodeDebugger : IWabbitcodeDebugger, IDisposable
    {
        private byte appPage;
        private List<WabbitcodeBreakpoint> breakpoints = new List<WabbitcodeBreakpoint>();
        private IWabbitemuDebugger debugger;
        private Dictionary<ListFileKey, ListFileValue> debugTable;
        private bool isAnApp;
        private bool isBreakpointed;
        private ushort oldSP = 0xFFFF;
        private bool showToolbar;
        private SymbolTable symTable;
        private Stack<int> stepStack = new Stack<int>();
        private bool disposed = false;

        #region Public Properties

        public byte AppPage
        {
            get
            {
                return this.appPage;
            }
            set
            {
                this.appPage = value;
            }
        }

        public Array Apps
        {
            get
            {
                return this.debugger.Apps;
            }
        }

        public List<WabbitcodeBreakpoint> Breakpoints
        {
            get
            {
                return this.breakpoints;
            }
            set
            {
                this.breakpoints = value;
            }
        }

        public IZ80 CPU
        {
            get
            {
                return this.debugger.CPU;
            }
        }

        public bool IsAnApp
        {
            get
            {
                return this.isAnApp;
            }
            set
            {
                this.isAnApp = value;
            }
        }

        public bool IsBreakpointed
        {
            get
            {
                return this.isBreakpointed;
            }
            set
            {
                this.isBreakpointed = value;
            }
        }

        public IKeypad Keypad
        {
            get
            {
                return this.debugger.Keypad;
            }
        }

        public ILCD LCD
        {
            get
            {
                return this.debugger.LCD;
            }
        }

        public bool ShowToolbar
        {
            get
            {
                return this.showToolbar;
            }
            set
            {
                this.showToolbar = value;
            }
        }

        public Stack<int> StepStack
        {
            get
            {
                return this.stepStack;
            }
        }

        public SymbolTable SymbolTable
        {
            get
            {
                return this.symTable;
            }
        }
        #endregion

        public WabbitcodeDebugger()
        {
            isBreakpointed = false;
        }

        // TODO: this all sucks
        public void AddBreakpoint(int lineNumber, string fileName)
        {
            WabbitcodeBreakpoint newBreakpoint = new WabbitcodeBreakpoint();
            if (this.debugTable != null)
            {
                ListFileValue value;
                ListFileKey key =  new ListFileKey(fileName.ToLower(), lineNumber + 1);
                this.debugTable.TryGetValue(key, out value);
                if (value == null)
                {
                    return;
                }

                newBreakpoint.Address = value.Address;
                newBreakpoint.Page = value.Page;
                newBreakpoint.IsRam = newBreakpoint.Address > 0x8000;
                if (this.isAnApp && !newBreakpoint.IsRam)
                {
                    newBreakpoint.Page = (byte)(this.appPage - newBreakpoint.Page);
                }
                IBreakpoint wabbitBreakpoint = this.debugger.SetBreakpoint(newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
                newBreakpoint.WabbitemuBreakpoint = wabbitBreakpoint;
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
            
            this.breakpoints.Add(newBreakpoint);
        }

        public void BreakpointHit(object sender, Debugger.BreakpointEventArgs e)
        {
            IBreakpoint breakEvent = e.Breakpoint;
            ushort address = breakEvent.Address.Address;
            int page = breakEvent.Address.Page.Index;
            int relativePage = this.isAnApp ? this.appPage - page : page;
            WabbitcodeBreakpoint breakpoint = this.FindBreakpoint(address, (byte)page, address > 0x8000);
            if (breakpoint == null)
            {
                return;
            }

            // throw new Exception("Breakpoint not found!");
            breakpoint.numberOfTimesHit++;
            bool conditionsTrue = breakpoint.Enabled;
            switch (breakpoint.hitCountCondition)
            {
            case HitCountEnum.BreakEqualTo:
                if (breakpoint.numberOfTimesHit != breakpoint.hitCountConditionNumber)
                {
                    conditionsTrue &= false;
                }
                break;
            case HitCountEnum.BreakGreaterThanEqualTo:
                if (breakpoint.numberOfTimesHit < breakpoint.hitCountConditionNumber)
                {
                    conditionsTrue &= false;
                }
                break;
            case HitCountEnum.BreakMultipleOf:
                if (breakpoint.numberOfTimesHit % breakpoint.hitCountConditionNumber != 0)
                {
                    conditionsTrue &= false;
                }
                break;
            }

            // breakpoint.breakCondition = new List<GlobalClass.BreakCondition>();
            // GlobalClass.BreakCondition newCondition = new GlobalClass.BreakCondition();
            // newCondition.h = 5 << 16;
            // newCondition.l = 5 << 16;
            // breakpoint.breakCondition.Add(newCondition);
            if (breakpoint.breakCondition != null)
            {
                foreach (BreakCondition condition in breakpoint.breakCondition)
                {
                    conditionsTrue &= breakpoint.EvalCondition(debugger.CPU, condition);
                }

                if (conditionsTrue)
                {
                    ListFileValue value = new ListFileValue(address, (byte)relativePage);
                    ListFileKey key;
                    this.debugTable.TryGetKey(value, out key); ;
                    if (key == null)
                    {
                        DockingService.ShowError("Unable to find breakpoint");
                        return;
                    }

                    DockingService.MainForm.Invoke(() =>
                    {
                        DocumentService.GotoLine(key.FileName, key.LineNumber);
                        DocumentService.HighlightDebugLine(key.LineNumber);
                        this.isBreakpointed = true;

                        // switch to back to us
                        DockingService.MainForm.Activate();
                        DockingService.MainForm.UpdateDebugStuff();
                        this.UpdateStack();
                        DockingService.MainForm.UpdateTrackPanel();
                        DockingService.MainForm.UpdateDebugPanel();
                    });
                }
                else
                {
                    this.isBreakpointed = false;
                    this.debugger.Running = true;
                }
            }
        }

        public void CancelDebug()
        {
            this.isAnApp = false;
            this.oldSP = 0xFFFF;
            DockingService.CallStack.Clear();
            if (this.debugger != null)
            {
                this.debugger.Running = false;
                this.debugger.Visible = false;
                this.debugger = null;
            }

            DockingService.MainForm.Invoke(() => DockingService.MainForm.CancelDebug());
            if (this.debugTable != null)
            {
                this.debugTable.Clear();
            }

            this.debugTable = null;
        }

        public void DoStep(StepType stepType)
        {
            // need to clear the old breakpoint so lets save it
            ushort currentPC = this.debugger.CPU.PC;
            byte oldPage = this.GetPageNum(currentPC);
            ListFileValue value = new ListFileValue(currentPC, oldPage);
            ListFileKey key;
            this.debugTable.TryGetKey(value, out key);
            ListFileKey newKey = key;
            while (newKey.LineNumber == key.LineNumber)
            {
                switch (stepType)
                {
                case StepType.Step:
                    this.debugger.Step();
                    break;
                case StepType.StepOver:
                    this.debugger.StepOver();
                    break;
                case StepType.StepOut:
                    this.debugger.StepOut();
                    break;
                }

                ushort address = this.debugger.CPU.PC;

                byte page = this.GetPageNum(address);
                value = new ListFileValue(address, page);
                if (this.debugTable == null)
                {
                    return;
                }
                if (this.debugTable.ContainsValue(value))
                {
                    if (page != oldPage && stepType == StepType.StepOver)
                    {
                        continue;
                    }

                    // need to get the new info
                    this.debugTable.TryGetKey(value, out newKey);
                }
            }

            this.isBreakpointed = true;
            DockingService.MainForm.Invoke(() => DockingService.MainForm.DoneStep(newKey));
        }

        public WabbitcodeBreakpoint FindBreakpoint(ushort address, byte page, bool isRam)
        {
            return this.FindBreakpoint(new WabbitcodeBreakpoint(address, page, isRam));
        }

        public WabbitcodeBreakpoint FindBreakpoint(WabbitcodeBreakpoint breakpoint)
        {
            foreach (WabbitcodeBreakpoint pointToCheck in this.breakpoints)
            {
                if (breakpoint == pointToCheck)
                {
                    return pointToCheck;
                }
            }

            return null;
        }

        public WabbitcodeBreakpoint FindBreakpoint(string file, int lineNumber)
        {
            return this.FindBreakpoint(new WabbitcodeBreakpoint(file, lineNumber));
        }

        public Image GetScreenImage()
        {
            return debugger.GetScreenImage();
        }

        public void GotoAddress(ushort address)
        {
            ListFileValue value = new ListFileValue(address, this.GetPageNum(address));
            if (!this.debugTable.ContainsValue(value))
            {
                return;
            }

            ListFileKey key;
            this.debugTable.TryGetKey(value, out key);
            DocumentService.GotoLine(key.FileName, key.LineNumber);
        }

        public bool IsBreakpointPossible(int lineNumber, string fileName)
        {
            return this.debugTable == null || this.debugTable.ContainsKey(new ListFileKey(fileName.ToLower(), (lineNumber + 1)));
        }

        public void RemoveBreakpoint(int lineNumber, string fileName)
        {
            WabbitcodeBreakpoint newBreakpoint = this.FindBreakpoint(fileName, lineNumber);
            if (newBreakpoint == null)
            {
                return;
            }

            if (this.debugger != null)
            {
                this.debugger.ClearBreakpoint(newBreakpoint.WabbitemuBreakpoint);
            }

            this.breakpoints.Remove(newBreakpoint);
        }

        public void SetPCToSelect(string fileName, int lineNumber)
        {
            ListFileKey key = new ListFileKey(fileName.ToLower(), lineNumber + 1);
            if (!this.debugTable.ContainsKey(key))
            {
                MessageBox.Show("Unable to set statement here!");
                return;
            }

            ListFileValue value;
            this.debugTable.TryGetValue(key, out value);
            this.debugger.CPU.PC = value.Address;
            byte page = value.Page;
            if (this.isAnApp)
            {
                page = (byte)(this.appPage - page);
                this.debugger.Memory.Bank[1] = this.debugger.Memory.Flash[page];
            }
            else
            {
                this.debugger.Memory.Bank[2] = this.debugger.Memory.RAM[1];
            }

            DocumentService.RemoveDebugHighlight();
            DocumentService.HighlightDebugLine(lineNumber + 1);
            DockingService.MainForm.UpdateDebugPanel();
        }

        public void Step()
        {
            this.isBreakpointed = false;
            DocumentService.HighlightCall();
            DoStep(StepType.Step);
            //Task.Factory.StartNew(() => );
        }

        public void StepOut()
        {
            this.isBreakpointed = false;
            DoStep(StepType.StepOut);
            //Task.Factory.StartNew(() => DoStep(StepType.StepOut));
        }

        public void StepOver()
        {
            this.isBreakpointed = false;
            DoStep(StepType.StepOver);
            //Task.Factory.StartNew(() => DoStep(StepType.Step));
        }

        public void UpdateStack()
        {
            int currentSP = this.debugger.CPU.SP;

            // if someone has changed sp we dont want a really big callstack
            if (currentSP < 0xFE66)
            {
                return;
            }

            int bytesToRead = 0xFFFF - currentSP;
            while (this.oldSP != currentSP - 2)
            {
                if (this.oldSP > currentSP - 2)
                {
                    DockingService.CallStack.AddStackData(this.oldSP, this.ReadShort(this.oldSP));
                    this.oldSP -= 2;
                }
                else
                {
                    DockingService.CallStack.RemoveLastRow();
                    this.oldSP += 2;
                }
            }
        }

        internal ListFileKey GetListKey(ushort address, byte page)
        {
            if (this.debugTable == null)
            {
                return null;
            }

            ListFileKey key;
            this.debugTable.TryGetKey(new ListFileValue(address, page), out key);
            return key;
        }

        internal ListFileValue GetListValue(string file, int lineNumber)
        {
            if (this.debugTable == null || file == null)
            {
                return null;
            }

            ListFileValue value;
            this.debugTable.TryGetValue(new ListFileKey(file.ToLower(), lineNumber), out value);
            return value;
        }

        internal byte GetPageNum(ushort address)
        {
            int page = this.appPage - debugger.Memory.Bank[address >> 14].Index;
            return (byte)page;
        }

        internal void InitDebugger(string outputFile)
        {
            this.debugger = new WabbitemuDebugger();
            this.debugger.LoadFile(outputFile);
            this.debugger.Visible = true;
            this.debugger.OnBreakpoint += BreakpointHit;
        }

        internal void Pause()
        {
            this.isBreakpointed = false;
            IntPtr calculatorHandle = DockingService.MainForm.Handle;

            // switch to back to us
            NativeMethods.SetForegroundWindow(calculatorHandle);
            DockingService.MainForm.UpdateDebugStuff();
            DockingService.MainForm.UpdateTrackPanel();
            DockingService.MainForm.UpdateDebugPanel();
            ushort currentPC = this.debugger.CPU.PC;
            ListFileValue value = new ListFileValue(currentPC, this.GetPageNum(currentPC));
            if (!this.debugTable.ContainsValue(value))
            {
                MessageBox.Show("Unable to pause here");
                return;
            }

            ListFileKey key;
            this.debugTable.TryGetKey(value, out key);
            DocumentService.GotoLine(key.FileName, key.LineNumber);
            DocumentService.HighlightDebugLine(key.LineNumber);
            this.debugger.Running = false;
        }

        internal byte ReadByte(ushort address)
        {
            return debugger.Memory.ReadByte(address);
        }

        internal ushort ReadShort(ushort address)
        {
            return debugger.Memory.ReadWord(address);
        }

        internal byte[] Read(ushort address, int count)
        {
            return (byte[])debugger.Memory.Read(address, (ushort) count);
        }

        internal void Run()
        {
            this.isBreakpointed = false;
            //IntPtr calculatorHandle = NativeMethods.FindWindow("z80", "Wabbitemu");
            //if (calculatorHandle == IntPtr.Zero)
            //{
            //    this.StartDebug();
            //    return;
            //}

            // switch to the emulator
            //NativeMethods.SetForegroundWindow(calculatorHandle);
            this.debugger.Running = true;
            if (DockingService.ActiveDocument != null)
            {
                DockingService.MainForm.UpdateDebugStuff();
                DocumentService.RemoveDebugHighlight();
                DockingService.ActiveDocument.Refresh();
            }
        }

        internal void SetBreakpoint(WabbitcodeBreakpoint newBreakpoint)
        {
            this.debugger.SetBreakpoint(newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
        }

        internal void SimulateKeyPress(Keys key)
        {
            this.debugger.Keypad.PressVirtKey((int)key);
            Thread.Sleep(50);
            this.debugger.Keypad.ReleaseVirtKey((int)key);
        }

        internal void StartWithoutDebug()
        {
            if (ProjectService.CurrentBuildConfig == null)
            {
                DockingService.ShowError("No config setup");
                return;
            }

            ThreadPool.QueueUserWorkItem(o => AssemblerService.Instance.AssembleProject(ProjectService.Project));
            AssemblerService.Instance.AssemblerProjectFinished += this.StartWithoutDebuggingAssemblerFinished;
        }

        private List<TextMarker> GetStaticLabels(string editorText)
        {
            int counter = 0, newCounter, textLength = editorText.Length;
            string possibleReference;
            List<TextMarker> markersToAdd = new List<TextMarker>();
            ParserService parserService = new ParserService();
            while (counter < textLength)
            {
                if (editorText[counter] == ';')
                {
                    while (editorText[counter] != '\n')
                    {
                        counter++;
                    }
                    newCounter = parserService.GetWord(editorText, counter);
                    if (newCounter == -1)
                    {
                        counter++;
                        continue;
                    }

                    possibleReference = editorText.Substring(counter, newCounter - counter);
                    if (!Settings.Default.caseSensitive)
                    {
                        possibleReference = possibleReference.ToUpper();
                    }

                    string address = this.SymbolTable.GetAddressFromLabel(possibleReference);
                    if (!string.IsNullOrEmpty(possibleReference) && address != null)
                    {
                        TextMarker marker = new TextMarker(counter, newCounter - counter, TextMarkerType.Invisible)
                        {
                            Tag = "Static Label",
                            ToolTip = address
                        };

                        markersToAdd.Add(marker);
                    }

                    counter += possibleReference.Length + 1;
                    while (counter < textLength && char.IsWhiteSpace(editorText[counter]))
                    {
                        counter++;
                    }
                }
            }

            return markersToAdd;
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

            this.AppPage = (byte)app.Value.Page.Index;
        }

        internal string ParseOutputFiles(IProject project)
        {
            string createdName;
            string fileName;
            string listName;
            string symName;
            if (project.ProjectOutputs.Count < 1)
            {
                throw new DebuggingException("No project outputs detected");;
            }

            createdName = project.ProjectOutputs[0];
            fileName = project.BuildSystem.MainFile;
            if (!Path.IsPathRooted(createdName))
            {
                createdName = FileOperations.NormalizePath(Path.Combine(project.ProjectDirectory, createdName));
            }

            if (project.ListOutputs.Count < 1)
            {
                throw new DebuggingException("Error missing list file");
            }

            listName = project.ListOutputs[0];
            if (project.LabelOutputs.Count < 1)
            {
                throw new DebuggingException("Error missing label file");
            }

            symName = project.LabelOutputs[0];

            StreamReader listReader = null;
            string listFileText, symFileText = String.Empty;
            try
            {
                listReader = new StreamReader(listName);
                listFileText = listReader.ReadToEnd();
            }
            catch (Exception)
            {
                throw new DebuggingException("Error parsing list file");
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
                symFileText = symReader.ReadToEnd();

            }
            catch (Exception)
            {
                throw new DebuggingException("Error parsing label file");
            }
            finally
            {
                if (symReader != null)
                {
                    symReader.Dispose();
                }
            }

            // TODO: logic
            this.isAnApp = true;
            this.debugTable = new Dictionary<ListFileKey, ListFileValue>();
            this.ParseListFile(listFileText, fileName, Path.GetDirectoryName(fileName));
            this.symTable = new SymbolTable();
            this.symTable.ParseSymFile(symFileText);
            return createdName;
        }

        private void ParseListFile(string listFileContents, string assembledFile, string projectPath)
        {
            string currentFile = assembledFile;
            string[] lines = listFileContents.Split('\n');
            int currentLine = 0;
            foreach (string line in lines)
            {
                if (string.IsNullOrEmpty(line) || line == "\r")
                {
                    continue;
                }
                if (line.Contains("Listing for file"))
                {
                    currentFile = Path.Combine(
                                      projectPath,
                                      line.Substring(line.IndexOf('\"') + 1,
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

                    // correction for ram pages
                    if (page == 0 && address >= 0x8000 && address < 0xC000)
                    {
                        page = 1;
                    }
                    ListFileKey key = new ListFileKey(currentFile, lineNumber);
                    ListFileValue value = new ListFileValue(address, page);

                    // shouldnt be necessary
                    if (!this.debugTable.ContainsKey(key))
                    {
                        this.debugTable.Add(key, value);
                    }
                }
            }
        }

        private void StartWithoutDebuggingAssemblerFinished(object sender, AssemblyFinishEventArgs e)
        {
            if (!e.AssemblySucceeded)
            {
                DockingService.ShowError("Assembly failed");
            }

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
                if (!potential.ProcessName.Contains("wabbitemu", StringComparison.OrdinalIgnoreCase))
                {
                    continue;
                }

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

        private void staticLabelsParser_DoWork(object sender, DoWorkEventArgs e)
        {
            string text;
            if (e.Argument == null)
            {
                foreach (NewEditor child in DockingService.Documents)
                {
                    text = child.EditorText;
                    List<TextMarker> markers = this.GetStaticLabels(text);
                    DockingService.MainForm.Invoke(() => child.AddMarkers(markers));
                }
            }
            else
            {
                NewEditor child = (NewEditor)e.Argument;
                text = child.EditorText;
                List<TextMarker> markers = this.GetStaticLabels(text);
                child.AddMarkers(markers);
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (disposing)
                {
                    debugger.Dispose();
                }
            }
            disposed = true;
        }
    }
}