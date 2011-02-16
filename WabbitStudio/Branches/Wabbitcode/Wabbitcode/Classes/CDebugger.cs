using System;
using Revsoft.Wabbitcode.Classes;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Docking_Windows;
using System.Collections;
using Revsoft.TextEditor.Document;
using System.IO;
using Revsoft.TextEditor;
using System.Threading;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Globalization;
using System.Drawing;
using System.Runtime.InteropServices;

namespace Revsoft.Wabbitcode.Classes
{
    public class Debugger
    {
        private string listName { get; set; }
        private string symName { get; set; }
        private string fileName { get; set; }
        private string startAddress { get; set; }
        private string createdName { get; set; }
        private Assembler assembler;
        public bool debugging { get; set; }
        private EmulatorWindow emulatorWindow { get; set; }
        public SymbolTableClass symTable { get; set; }
        //private Revsoft.Docking.DockPanel dockPanel;
        private byte apppage;
        private TextMarker highlight;
        private bool isAnApp;
        private Hashtable debugTable;
        private Hashtable debugTableReverse;
        private bool showToolbar = true;

        public Debugger()
        {
            debugging = true;
            GlobalClass.mainForm.updateDebugStuff(true);
            if (GlobalClass.project.projectOpen)
            {
                AssembleProject();
            }
            else
            {
                AssembleFile();
            }
            //wait for spasm to finish
            assembler.Join();
            //if errors, alert the user
            if (!debugging || assembler.Errors)
            {
                if (MessageBox.Show("There were errors assembling. Would you like to continue and try to debug?",
                                    "Continue", MessageBoxButtons.YesNo, MessageBoxIcon.Error) == DialogResult.No)
                {
                    GlobalClass.mainForm.cancelDebug_Click(null, null);
                    return;
                }
            }
            emulatorWindow = new EmulatorWindow();
            emulatorWindow.Show(GlobalClass.mainForm.dockPanel);

            GlobalClass.emulator.LoadFile(createdName);
            //return;

            showToolbar = Settings.Default.debugToolbar;
            Settings.Default.debugToolbar = true;
            if (!showToolbar)
                GlobalClass.mainForm.toolBarManager.AddControl(GlobalClass.mainForm.debugToolStrip, DockStyle.Top, 
                                                                    GlobalClass.mainForm.mainToolBar, DockStyle.Right);
            GlobalClass.mainForm.debugToolStrip.Height = GlobalClass.mainForm.mainToolBar.Height;
            GlobalClass.mainForm.updateChecks();
            GlobalClass.debugPanel.Show(GlobalClass.mainForm.dockPanel);
            GlobalClass.trackWindow.Show(GlobalClass.mainForm.dockPanel);
            GlobalClass.callStack.Show(GlobalClass.mainForm.dockPanel);

            StreamReader reader = new StreamReader(listName);
            string listFileText = reader.ReadToEnd();
            reader = new StreamReader(symName);
            string symFileText = reader.ReadToEnd();

            debugTable = new Hashtable();
            debugTableReverse = new Hashtable();
            parseListFile(listFileText, fileName, Path.GetDirectoryName(fileName), ref debugTable, ref debugTableReverse);
            symTable = new SymbolTableClass();
            symTable.parseSymFile(symFileText);
            //updateStack();
            if (startAddress == "4080")
            {
                isAnApp = true;
                Applist appList = new Applist();
                //TODO: SEARCH FOR APP
                /*foreach (Wabbitemu.AppEntry app in appList)
                {
                    
                }*/
                /*while (appList.count == 0)
                {
                    appList = GlobalClass.emulator.GetApplist();
                    Thread.Sleep(1000);
                }
                apppage = (byte)appList.apps[0].page;*/
                apppage = 105;

                TextEditorControl editorBox;
                GlobalClass.mainForm.staticLabelMarkers = new List<TextMarker>();
                foreach (newEditor child in GlobalClass.mainForm.MdiChildren)
                {
                    editorBox = child.editorBox;
                    string breakLocInfo;
                    ReadOnlyCollection<Breakpoint> marks = editorBox.Document.BreakpointManager.Marks;
                    foreach (Breakpoint breakpoint in marks)
                    {
                        int breakNum = GlobalClass.findBreakpoint(editorBox.FileName, breakpoint.LineNumber);
                        if (debugTable.ContainsValue(editorBox.FileName.ToLower() + ":" + (breakpoint.LineNumber + 1)) && breakNum != -1)
                        {
                            GlobalClass.WabbitcodeBreakpoint newBreakpoint = GlobalClass.breakpoints[breakNum];
                            breakLocInfo =
                                debugTableReverse[editorBox.FileName.ToLower() + ":" + (breakpoint.LineNumber + 1)].ToString();
                            newBreakpoint.Address = UInt16.Parse(breakLocInfo.Substring(3, 4), NumberStyles.HexNumber);
                            if (isAnApp)
                                newBreakpoint.Page = (byte)(apppage - byte.Parse(breakLocInfo.Substring(0, 2), NumberStyles.HexNumber));
                            else
                                newBreakpoint.Page = byte.Parse(breakLocInfo.Substring(0, 2), NumberStyles.HexNumber);
                            newBreakpoint.IsRam = newBreakpoint.Address > 0x8000;
                            newBreakpoint.file = editorBox.FileName;
                            newBreakpoint.lineNumber = breakpoint.LineNumber;
                            GlobalClass.breakpoints[breakNum] = newBreakpoint;
                            emulatorWindow.emulator.SetBreakpoint(GlobalClass.mainForm.Handle, newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
                        }
                        else
                        {
                            editorBox.Document.BreakpointManager.RemoveMark(breakpoint);
                            break;
                        }
                    }
                    child.setNextStateMenuItem.Visible = true;
                }
                if (!GlobalClass.mainForm.staticLabelsParser.IsBusy && !GlobalClass.mainForm.IsDisposed && !GlobalClass.mainForm.Disposing)
                    GlobalClass.mainForm.staticLabelsParser.RunWorkerAsync();
                //GlobalClass.breakManager.updateManager();
                #region OldDebug
                //Kept for sentimental reasons
                //((Wabbitcode.newEditor)(ActiveMdiChild)).editorBox.ActiveTextAreaControl.Enabled = false;

                //old stuff?
                //string locInfo = debugTable[page + ":" + startAddress].ToString();
                //string file = locInfo.Substring(0, locInfo.LastIndexOf(':'));
                //string line = locInfo.Substring(locInfo.LastIndexOf(':') + 1, locInfo.Length - locInfo.LastIndexOf(':') - 1);
                /*
                if (Path.GetExtension(createdName) == ".8xk")
                {
                    isAnApp = true;
                    Wabbitemu.AppEntry[] appList = debugger.getAppList();
                    apppage = appList[0].page;
                    debugger.setBreakpoint(false, apppage, 0x4080);
                }
                else
                {
                    debugger.sendKeyPress((int)Keys.F12);
                    debugger.releaseKeyPress((int)Keys.F12);
                    System.Threading.Thread.Sleep(2000);
                    debugger.setBreakpoint(true, 1, 0x9D95);
                }*/
                /*try
                {
                    while (debugging)
                    {
                        Application.DoEvents();
                    }
                
                        //calcScreen.Image = debugger.DrawScreen();
                        var currentLoc = new Wabbitemu.breakpoint();
                        currentLoc.Address = debugger.getState().PC;
                        currentLoc.Page = byte.Parse(getPageNum(currentLoc.Address.ToString("X")), NumberStyles.HexNumber);
                        currentLoc.IsRam = getRamState(currentLoc.Address.ToString("X"));
                        bool breakpointed = false;
                        foreach (Wabbitemu.breakpoint breakpoint in breakpoints)
                        {
                            if (breakpoint.Page == currentLoc.Page && breakpoint.Address == currentLoc.Address)
                                breakpointed = true;
                        }
                        while (breakpointed)
                        {
                            //updateRegisters();
                            //updateFlags();
                            //updateCPUStatus();
                            //updateInterrupts();
                            if (stepOverClicked)
                                step(debugTable);
                            Application.DoEvents();
                        }
                        debugger.step();
                        //System.Threading.Thread.Sleep(2000);
                        Application.DoEvents();
                    }
                }
                catch (COMException ex)
                {
                    if (ex.ErrorCode != -2147023174)
                        MessageBox.Show(ex.ToString());
                }*/
                #endregion
            }
        }

        private void AssembleProject()
        {
            assembler = new Assembler();
            listName = Path.Combine(GlobalClass.project.projectLoc, GlobalClass.project.projectName + ".lst");
            symName = Path.ChangeExtension(listName, ".lab");
            fileName = Path.ChangeExtension(listName, ".asm");
            int outputType = GlobalClass.project.getOutputType();
            createdName = Path.ChangeExtension(listName, Assembler.getExtension(outputType));
            startAddress = outputType == 5 ? "4080" : "9D95";
        }

        private bool AssembleFile()
        {
            if (GlobalClass.activeChild != null)
                fileName = GlobalClass.activeChild.editorBox.FileName;
            if (string.IsNullOrEmpty(fileName))
            {
                if (MessageBox.Show("Would you like to save this file?", "Save", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    GlobalClass.mainForm.SaveDocument(GlobalClass.activeChild);
                    fileName = GlobalClass.activeChild.editorBox.FileName;
                }
                else
                {
                    //GlobalClass.mainForm.EndDebug();
                    return false;
                }
            }
            assembler = new Assembler();
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
                    MessageBox.Show("You cannont debug a non 83/84 Plus file!");
                    //EndDebug();
                    return false;
            }
            return true;
        }

        public void gotoAddress(string address)
        {
            if (!debugTable.Contains(getPageNum(address) + ":" + address))
                return;
            string locInfo = debugTable[getPageNum(address) + ":" + address].ToString();
            string file = locInfo.Substring(0, locInfo.LastIndexOf(':'));
            string lineNumber = locInfo.Substring(locInfo.LastIndexOf(':') + 1,
                                                  locInfo.Length - locInfo.LastIndexOf(':') - 1);
            GlobalClass.mainForm.gotoLine(file, Convert.ToInt32(lineNumber) - 1);
        }

        private string getPageNum(string address)
        {
            int addressLoc = int.Parse(address, NumberStyles.HexNumber);
            string page = "00";
            Emulator.MEMSTATE memstate = GlobalClass.emulator.GetMemState();
            if (addressLoc < 0x4000) // && memstate.ram0 == 0)
                page = memstate.page0.ToString("X2");
            if (addressLoc >= 0x4000 && addressLoc < 0x8000) // && memstate.ram1 == 0)
                page = ((int)apppage - memstate.page1).ToString("X2");
            if (addressLoc >= 0x8000 && addressLoc < 0xC000) // && memstate.ram2 == 0)
                page = memstate.page2.ToString("X2");
            if (addressLoc >= 0xC000 && addressLoc < 0x10000) // && memstate.ram3 == 0)
                page = memstate.page3.ToString("X2");
            return page;
        }

        public void SetPCToSelect(string fileName, int lineNumber)
        {
            if (!debugTableReverse.Contains(fileName.ToLower() + ":" + (lineNumber + 1)))
            {
                MessageBox.Show("Unable to set statement here!");
                return;
            }
            string newPC = debugTableReverse[fileName.ToLower() + ":" + (lineNumber + 1)].ToString();
            Emulator.Z80_State state = emulatorWindow.emulator.GetState();
            Emulator.MEMSTATE memState = emulatorWindow.emulator.GetMemState();

            state.PC = UInt16.Parse(newPC.Substring(3, 4), NumberStyles.HexNumber);
            byte page = byte.Parse(newPC.Substring(0, 2), NumberStyles.HexNumber);
            if (isAnApp)
            {
                page = (byte)(apppage - page);
                memState.page1 = page;
                memState.ram1 = 0;
            }
            else
            {
                memState.page2 = 1;
                memState.ram2 = 1;
            }
            removeHighlight();
            highlightLine(lineNumber + 1);

            emulatorWindow.emulator.SetState(state);
            emulatorWindow.emulator.SetMemState(memState);
            GlobalClass.debugPanel.updateRegisters();
        }

        public void Pause()
        {
            string pagenum = getPageNum(GlobalClass.emulator.GetState().PC.ToString("X"));
            if (!debugTable.Contains(pagenum + ":" + GlobalClass.emulator.GetState().PC.ToString("X")))
            {
                MessageBox.Show("Unable to pause here");
                return;
            }
            string locInfo = debugTable[pagenum + ":" + GlobalClass.emulator.GetState().PC.ToString("X")].ToString();
            string file = locInfo.Substring(0, locInfo.LastIndexOf(':'));
            string lineNumber = locInfo.Substring(locInfo.LastIndexOf(':') + 1,
                                                  locInfo.Length - locInfo.LastIndexOf(':') - 1);
            GlobalClass.mainForm.gotoLine(file, Convert.ToInt32(lineNumber));
            highlightLine(Convert.ToInt32(lineNumber));
        }

        public void removeHighlight()
        {
            if (GlobalClass.activeChild == null || highlight == null)
                return;
            foreach (newEditor child in GlobalClass.mainForm.MdiChildren)
                if (child.editorBox.FileName == highlight.Tag)
                {
                    child.Show();
                    break;
                }
            GlobalClass.activeChild.editorBox.Document.MarkerStrategy.RemoveMarker(highlight);
        }

        private void highlightLine(int newLineNumber)
        {
            //this code highlights the current line
            //I KNOW IT WORKS DONT FUCK WITH IT
            TextEditorControl editorBox = GlobalClass.activeChild.editorBox;
            TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;
            editorBox.ActiveTextAreaControl.ScrollTo(newLineNumber - 1);
            editorBox.ActiveTextAreaControl.Caret.Line = newLineNumber - 1;
            int start = textArea.Caret.Offset == editorBox.Text.Length ? textArea.Caret.Offset - 1 : textArea.Caret.Offset;
            int length = editorBox.Document.TextContent.Split('\n')[textArea.Caret.Line].Length;
            if (textArea.Document.TextContent[start] == '\n')
                start--;
            while (start > 0 && textArea.Document.TextContent[start] != '\n')
                start--;
            start++;
            while (start < textArea.Document.TextContent.Length && (textArea.Document.TextContent[start] == ' ' || textArea.Document.TextContent[start] == '\t'))
            {
                start++;
                length--;
            }
            if (length >= editorBox.Text.Length)
                length += (editorBox.Text.Length - 1) - length;
            if (editorBox.Text.IndexOf(';', start, length) != -1)
                length = editorBox.Text.IndexOf(';', start, length) - start - 1;
            if (editorBox.Text.Length <= start + length)
                length--;
            while (editorBox.Text[start + length] == ' ' || editorBox.Text[start + length] == '\t')
                length--;
            length++;
            highlight = new TextMarker(start, length, TextMarkerType.SolidBlock, Color.Yellow, Color.Black) { Tag = editorBox.FileName };
            editorBox.Document.MarkerStrategy.AddMarker(highlight);
            editorBox.Refresh();
        }

        private void HitBreakpoint()
        {
            ushort address = GlobalClass.emulator.GetState().PC;
            string currentPC = address.ToString("X");
            string pagenum = getPageNum(currentPC);
            byte page = Convert.ToByte(pagenum);
            if (isAnApp)
                page = (byte)(apppage - page);
            int breakNum = GlobalClass.findBreakpoint(address, page, address > 0x8000);
            if (breakNum == -1)
                return;
            GlobalClass.WabbitcodeBreakpoint breakpoint = GlobalClass.breakpoints[breakNum];
            breakpoint.numberOfTimesHit = GlobalClass.breakpoints[breakNum].numberOfTimesHit + 1;
            bool conditionsTrue = breakpoint.Enabled;
            switch (breakpoint.hitCountCondition)
            {
                case GlobalClass.HitCountEnum.BreakEqualTo:
                    if (breakpoint.numberOfTimesHit != breakpoint.hitCountConditionNumber)
                        conditionsTrue &= false;
                    break;
                case GlobalClass.HitCountEnum.BreakGreaterThanEqualTo:
                    if (breakpoint.numberOfTimesHit < breakpoint.hitCountConditionNumber)
                        conditionsTrue &= false;
                    break;
                case GlobalClass.HitCountEnum.BreakMultipleOf:
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
                foreach (GlobalClass.BreakCondition condition in breakpoint.breakCondition)
                    conditionsTrue &= evalCondition(condition);
            if (conditionsTrue)
            {
                string locInfo = debugTable[pagenum + ":" + currentPC].ToString();
                string file = locInfo.Substring(0, locInfo.LastIndexOf(':'));
                string lineNumber = locInfo.Substring(locInfo.LastIndexOf(':') + 1,
                                                      locInfo.Length - locInfo.LastIndexOf(':') - 1);
                GlobalClass.breakpoints[breakNum] = breakpoint;
                GlobalClass.mainForm.gotoLine(file, Convert.ToInt32(lineNumber));
                highlightLine(Convert.ToInt32(lineNumber));

                //this reinitiates all the good stuff
                IntPtr calculatorHandle = GlobalClass.mainForm.Handle;
                //switch to back to us
                SetForegroundWindow(calculatorHandle);
                GlobalClass.mainForm.updateDebugStuff(true);
                GlobalClass.trackWindow.updateVars();
                GlobalClass.debugPanel.updateFlags();
                GlobalClass.debugPanel.updateRegisters();
                GlobalClass.debugPanel.updateScreen();
            }
            else
                GlobalClass.emulator.RunCalc();
        }
        [DllImport("user32.dll")]
        private static extern bool SetForegroundWindow(IntPtr hWnd);

        public void Step(bool stepover)
        {
            Emulator.Z80_State state;
            foreach (newEditor child in GlobalClass.mainForm.MdiChildren)
                /*if (child.DocumentChanged)
                {
                    if (!isAnApp)
                    {
                        MessageBox.Show("Edit and continue is not available for programs.\nRestart your debugging session.", "Unable to continue", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }
                    //if (!editAndContinue())
                    //	return;
                    string pageandpc = debugTableReverse[child.editorBox.FileName.ToLower() + ":" + child.editorBox.Document.GetLineNumberForOffset(highlight.Offset) + 1].ToString();
                    ushort address = Convert.ToUInt16(pageandpc.Substring(3, 4), 16);
                    //byte page = Convert.ToByte(pageandpc.Substring(4, 2), 16);
                    state = GlobalClass.emulator.GetState();
                    state.PC = address;
                    GlobalClass.emulator.SetState(state);
                    // CWabbitemu.MEMSTATE mem = debugger.getMemState();
                }*/
            removeHighlight();
            //need to clear the old breakpoint so lets save it
            string locInfo = debugTable[getPageNum(GlobalClass.emulator.GetState().PC.ToString("X")) + ":" +
                                                    GlobalClass.emulator.GetState().PC.ToString("X")].ToString();
            string lineNumber = locInfo.Substring(locInfo.LastIndexOf(':') + 1,
                                                  locInfo.Length - locInfo.LastIndexOf(':') - 1);
            string newLineNumber = lineNumber;
            string newFile = locInfo.Substring(0, locInfo.LastIndexOf(':'));
            string newLocInfo;
            //int counter = 0;
            while (newLineNumber == lineNumber)
            {
                if (stepover)
                    GlobalClass.emulator.StepOver();
                else
                    GlobalClass.emulator.Step();
                state = GlobalClass.emulator.GetState();


                string page = getPageNum(state.PC.ToString("X"));
                string address = state.PC.ToString("X4");
                address = page + ":" + address;
                if (debugTable.ContainsKey(address))
                {
                    //need to get the new info
                    newLocInfo = debugTable[address].ToString();
                    newFile = newLocInfo.Substring(0, newLocInfo.LastIndexOf(':'));
                    newLineNumber = newLocInfo.Substring(newLocInfo.LastIndexOf(':') + 1,
                                                         newLocInfo.Length - newLocInfo.LastIndexOf(':') - 1);
                }
                else
                    newLineNumber = lineNumber;
            }
            stepover = false;
            updateStack();
            GlobalClass.mainForm.gotoLine(newFile, Convert.ToInt32(newLineNumber));
            highlightLine(Convert.ToInt32(newLineNumber));
            GlobalClass.trackWindow.updateVars();
            GlobalClass.debugPanel.updateFlags();
            GlobalClass.debugPanel.updateRegisters();
            GlobalClass.debugPanel.updateScreen();
        }

        private ushort oldSP = 0xFFFF;
        private void updateStack()
        {
            int currentSP = GlobalClass.emulator.GetState().SP;
            //oldSP = 0xFFFF;
            //callStack.Clear();
            if (currentSP < 0xFE66)
                return;
            while (oldSP != currentSP - 2)
            {
                if (oldSP > currentSP - 2)
                {
                    GlobalClass.callStack.addStackData(oldSP, GlobalClass.emulator.ReadMem(oldSP) +
                                                    GlobalClass.emulator.ReadMem((ushort)(oldSP + 1)) * 256);
                    oldSP -= 2;
                }
                else
                {
                    GlobalClass.callStack.removeLastRow();
                    oldSP += 2;
                }
            }
        }

        private bool evalCondition(GlobalClass.BreakCondition condition)
        {
            bool isTrue = true;
            Emulator.Z80_State state = GlobalClass.emulator.GetState();
            if (condition.a >= 0xFFFF)
                isTrue &= ((state.AF >> 16) == (ushort)(condition.a >> 8));
            if (condition.b >= 0xFFFF)
                isTrue &= ((state.BC >> 16) == (ushort)(condition.b >> 8));
            if (condition.c >= 0xFFFF)
                isTrue &= ((state.BC & 0xFF) == (ushort)(condition.c >> 16));
            if (condition.d >= 0xFFFF)
                isTrue &= ((state.DE >> 16) == (ushort)(condition.d >> 8));
            if (condition.e >= 0xFFFF)
                isTrue &= ((state.DE & 0xFF) == (ushort)(condition.e >> 16));
            if (condition.h >= 0xFFFF)
                isTrue &= (state.HL >> 8 == (ushort)(condition.h >> 16));
            if (condition.l >= 0xFFFF)
                isTrue &= ((state.HL & 0xFF) == (ushort)(condition.l >> 16));
            if (condition.ix >= 0xFFFF)
                isTrue &= (state.IX == (ushort)condition.ix);
            if (condition.iy >= 0xFFFF)
                isTrue &= (state.IY == (ushort)(condition.iy >> 16));
            if (condition.sp >= 0xFFFF)
                isTrue &= (state.SP == (ushort)(condition.sp >> 16));
            if (condition.cFlag > 2)
                isTrue &= ((state.AF & 1) == condition.cFlag);
            if (condition.nFlag >= 2)
                isTrue &= ((state.AF & 2) == condition.nFlag);
            if (condition.pvFlag >= 2)
                isTrue &= ((state.AF & 4) == condition.pvFlag);
            if (condition.hFlag >= 2)
                isTrue &= ((state.AF & 16) == condition.hFlag);
            if (condition.zFlag >= 2)
                isTrue &= ((state.AF & 64) == condition.zFlag);
            if (condition.sFlag >= 2)
                isTrue &= ((state.AF & 128) == condition.sFlag);
            return isTrue;
        }

        public string GetDebugTable(string fileName, int lineNum)
        {
            if (debugTable == null || fileName == null || !debugTable.ContainsValue(fileName.ToLower() + ":" + (lineNum + 1)))
                return "";
            return debugTableReverse[fileName.ToLower() + ":" + (lineNum + 1)].ToString();

        }

        public void addBreakpoint(int lineNumber, string fileName)
        {
            GlobalClass.WabbitcodeBreakpoint newBreakpoint = new GlobalClass.WabbitcodeBreakpoint();
            if (!debugging)
            {
                newBreakpoint.IsRam = false;
                newBreakpoint.Address = 0;
                newBreakpoint.Page = 0;
            }
            else
            {
                string breakLocInfo = debugTableReverse[fileName.ToLower() + ":" + (lineNumber + 1)].ToString();
                newBreakpoint.Address = UInt16.Parse(breakLocInfo.Substring(3, 4), NumberStyles.HexNumber);
                newBreakpoint.Page = byte.Parse(breakLocInfo.Substring(0, 2), NumberStyles.HexNumber);
                if (isAnApp)
                    newBreakpoint.Page = (byte)(apppage - newBreakpoint.Page);
                newBreakpoint.IsRam = newBreakpoint.Address > 0x8000;
                GlobalClass.emulator.SetBreakpoint(GlobalClass.mainForm.Handle, newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
            }
            newBreakpoint.file = fileName;
            newBreakpoint.lineNumber = lineNumber;
            newBreakpoint.Enabled = true;
            GlobalClass.breakpoints.Add(newBreakpoint);
        }

        public bool breakpointExists(int lineNumber, string fileName)
        {
            return GlobalClass.debugger.debugTableReverse == null || GlobalClass.debugger.debugTableReverse.ContainsKey(fileName.ToLower() + ":" + (lineNumber + 1));
        }

        public void removeBreakpoint(int lineNumber, string fileName)
        {
            int breakNum = GlobalClass.findBreakpoint(fileName, lineNumber);
            if (breakNum == -1)
                return;
            GlobalClass.WabbitcodeBreakpoint newBreakpoint = GlobalClass.breakpoints[breakNum];
            if (GlobalClass.debugger.debugging)
            {
                //int page = newBreakpoint.Page;
                //if (isAnApp)
                //    page = (byte) (apppage - newBreakpoint.Page);
                GlobalClass.emulator.ClearBreakpoint(newBreakpoint.IsRam, newBreakpoint.Page, newBreakpoint.Address);
            }
            GlobalClass.breakpoints.Remove(newBreakpoint);
        }

        private void parseListFile(string listFileContents, string assembledFile, string projectPath,
                                   ref Hashtable debugginz, ref Hashtable reverseDebugginz)
        {
            string currentFile = assembledFile;
            string[] lines = listFileContents.Split('\n');
            bool addLine = false;
            int currentLine = 0;
            string newFile = "";
            int tempFileNum = 0;
            foreach (string line in lines)
            {
                if (line == "" || line == "\r")
                    break;
                if (line.Contains("Listing for file"))
                {
                    if (addLine)
                    {
                        var writer = new StreamWriter(currentFile);
                        writer.Write(newFile);
                        writer.Flush();
                        writer.Close();
                        tempFileNum++;
                    }
                    currentFile = Path.Combine(projectPath,
                                               line.Substring(line.IndexOf('\"') + 1,
                                                              line.LastIndexOf('\"') - line.IndexOf('\"') - 1));
                    if (GlobalClass.project.projectOpen)
                        addLine = false;//!IsFileInProject(currentFile);
                    else
                        addLine = false;// !File.Exists(currentFile);
                    if (addLine)
                        currentFile += tempFileNum.ToString();
                }
                if (line.Substring(0, 5) != "     " && line.Substring(13, 12) != "            " &&
                    line.Substring(13, 12) != " -  -  -  - " && !line.Contains("Listing for file"))
                {
                    while (currentLine < Convert.ToInt32(line.Substring(0, 5).Trim()))
                    {
                        currentLine++;
                        if (addLine)
                            newFile += '\n';
                    }
                    if (addLine)
                        newFile += line.Substring(26, line.Length - 27) + '\n';
                    string temp = line.Substring(6, 7);
                    if (temp.StartsWith("00") &&
                        int.Parse(temp.Substring(3, 4), NumberStyles.HexNumber) >= 0x8000 && int.Parse(temp.Substring(3, 4), NumberStyles.HexNumber) < 0xC000)
                        temp = "01" + temp.Substring(2, 5);
                    if (!debugginz.ContainsKey(temp))
                        debugginz.Add(temp, currentFile.ToLower() + ':' + line.Substring(0, 5).Trim());
                    if (!reverseDebugginz.ContainsKey(currentFile.ToLower() + ':' + line.Substring(0, 5).Trim()))
                        reverseDebugginz.Add(currentFile.ToLower() + ':' + line.Substring(0, 5).Trim(), temp);
                }
            }
        }

        public void EndDebug()
        {
            Settings.Default.debugToolbar = showToolbar;
            if (!showToolbar)
                GlobalClass.mainForm.toolBarManager.RemoveControl(GlobalClass.mainForm.debugToolStrip);
            GlobalClass.mainForm.updateChecks();
            debugging = false;
            isAnApp = false;
            if (emulatorWindow != null)
                emulatorWindow.Close();
            foreach (newEditor child in GlobalClass.mainForm.MdiChildren)
            {
                TextEditorControl editorBox = child.editorBox;
                editorBox.Document.MarkerStrategy.RemoveMarker(highlight);
                editorBox.Document.MarkerStrategy.RemoveAll(InvisibleMarkers);

                child.setNextStateMenuItem.Visible = false;
            }
        }

        private static bool InvisibleMarkers(TextMarker marker)
        {
            return marker.TextMarkerType == TextMarkerType.Invisible;
        }
    }
}
