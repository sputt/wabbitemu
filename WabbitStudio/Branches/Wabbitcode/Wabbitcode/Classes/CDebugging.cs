using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Properties;

namespace Revsoft.Wabbitcode.Classes
{
    class DebuggingClass
    {
        public DebuggingClass()
        {

        }

        private bool projectOpen;
        private byte apppage;
        private string wcodeProjectFile;
        //private int currentListLine;
        public Wabbitemu debugger;
        public bool debugging;
        private TextMarker highlight;
        private bool isAnApp;
        private List<Wabbitemu.breakpoint> breakpoints;
        private Hashtable debugTable;
        private Hashtable debugTableReverse;
        bool showToolbar = true;

        private void startDebug(object sender, EventArgs e)
        {
            if (debugging)
                run();
            else
            {
                debugging = true;
                string listName;
                string symName;
                string fileName = "";
                string startAddress;
                string createdName = "";
                bool error = true;
                if (projectOpen)
                {
                    listName = Path.Combine(projectLoc, projectName + ".lst");
                    symName = Path.Combine(projectLoc, projectName + ".lab");
                    fileName = Path.Combine(projectLoc, projectName + ".asm");
                    NewProject project = new NewProject(wcodeProjectFile);
                    int outputType = project.getOutputType();
                    startAddress = outputType == 5 ? "4080" : "9D95";
                    XmlNodeList buildConfigs;
                    XmlDocument doc = new XmlDocument();
                    doc.Load(wcodeProjectFile);
                    buildConfigs = doc.ChildNodes[1].ChildNodes[1].ChildNodes;
                    int counter = 0;
                    foreach (XmlNode config in buildConfigs)
                    {
                        if (config.Name.ToLower() == "debug")
                        {
                            Settings.Default.buildConfig = counter;
                            counter = -1;
                            break;
                        }
                        counter++;
                    }
                    if (counter == -1)
                        createdName = assembleProject();
                    else
                    {
                        MessageBox.Show("No build config named Debug was found!");
                        debugging = false;
                        return;
                    }
                }
                else
                {
                    if (ActiveMdiChild != null)
                        fileName = ((newEditor)(ActiveMdiChild)).editorBox.FileName;
                    error &= createListing(fileName, Path.ChangeExtension(fileName, "lst"));
                    //if (error)
                    //    MessageBox.Show("Problem creating list file");
                    error &= createSymTable(fileName, Path.ChangeExtension(fileName, "lab"));
                    //if (error)
                    //    MessageBox.Show("Problem creating symtable");
                    error &= assembleCode(fileName, false, Path.ChangeExtension(fileName, getExtension(Settings.Default.outputFile)));
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
                            MessageBox.Show("You cannont debug a non 83/84 Plus file!");
                            goto NoMoreDebug;
                    }
                }
                if (!debugging || !error)
                {
                    if (MessageBox.Show("There were errors compiling!!! Would you like to continue and try to debug?", "Continue", MessageBoxButtons.YesNo, MessageBoxIcon.Error) == DialogResult.No)
                    {
                        cancelDebug_Click(null, null);
                        return;
                    }
                }
                //if (!File.Exists(listName))
                //    createListing(fileName, Path.ChangeExtension(fileName, "lst"));
                debugger = new Wabbitemu(createdName);

                showToolbar = Settings.Default.debugToolbar;
                Settings.Default.debugToolbar = true;
                debugToolStrip.Show();
                updateChecks();
                debugPanel = new DebugPanel(debugger);
                debugPanel.Show(dockPanel, Settings.Default.debugPanelLoc);
                trackWindow.Show(dockPanel);

                StreamReader reader = new StreamReader(listName);
                //StreamReader breakReader = new StreamReader(fileName.Remove(fileName.Length - 3) + "brk");
                string listFileText = reader.ReadToEnd();
                //string[] listFileLines = listFileText.Split('\n');
                reader = new StreamReader(symName);
                string symFileText = reader.ReadToEnd();
                debugTable = new Hashtable();
                debugTableReverse = new Hashtable();
                TextEditorControl editorBox;
                //int CurrentListLine = 0;
                //while (!listFileLines[currentListLine].Contains("9D95"))
                //    currentListLine++;
                parseListFile(listFileText, fileName, Path.GetDirectoryName(fileName), ref debugTable, ref debugTableReverse);
                parseSymFile(symFileText);
                //string[] breakpoints = breakReader.ReadToEnd().Split('\n');
                //calcScreen.BackColor = Color.FromArgb(158, 171, 136);

                staticLabelMarkers = new List<TextMarker>();
                breakpoints = new List<Wabbitemu.breakpoint>();
                foreach (newEditor child in MdiChildren)
                {
                    editorBox = child.editorBox;
                    string breakLocInfo;
                    ReadOnlyCollection<Breakpoint> marks = editorBox.Document.BreakpointManager.Marks;
                    foreach (Breakpoint breakpoint in marks)
                    {
                        var newBreakpoint = new Wabbitemu.breakpoint();
                        if (debugTable.ContainsValue(editorBox.FileName + ":" + (breakpoint.LineNumber + 1)))
                        {
                            breakLocInfo =
                                debugTableReverse[editorBox.FileName + ":" + (breakpoint.LineNumber + 1)].ToString();
                            newBreakpoint.Address = UInt16.Parse(breakLocInfo.Substring(3, 4), NumberStyles.HexNumber);
                            if (isAnApp)
                                newBreakpoint.Page = (byte)(apppage - byte.Parse(breakLocInfo.Substring(0, 2), NumberStyles.HexNumber));
                            else
                                newBreakpoint.Page = byte.Parse(breakLocInfo.Substring(0, 2), NumberStyles.HexNumber);
                            newBreakpoint.IsRam = newBreakpoint.Address < 0x8000 ? 0 : 1;
                            breakpoints.Add(newBreakpoint);
                            debugger.setBreakpoint(Handle, newBreakpoint);
                        }
                        else
                            editorBox.Document.BreakpointManager.RemoveMark(breakpoint);
                    }
                    child.setNextStateMenuItem.Visible = true;
                    addStaticLabels(editorBox);
                }

                //debugger.setBreakpoint(Handle, true, 1, 0x9D95);
                Wabbitemu.MEMSTATE test = debugger.getMemState();
                if (!debugger.isWabbitOpen)
                    wabbitClosed();
                if (startAddress == "4080")
                {
                    isAnApp = true;
                    Wabbitemu.AppEntry[] appList = new Wabbitemu.AppEntry[20];
                    foreach (Wabbitemu.AppEntry app in appList)
                    {

                    }
                    while (appList[0].page_count == 0)
                    {
                        appList = debugger.getAppList();
                        apppage = (byte)appList[0].page;
                        Thread.Sleep(500);
                    }
                }
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
        NoMoreDebug:
            ;
        }

        public void SetPCToSelect(string fileName, int lineNumber)
        {
            string newPC = debugTableReverse[fileName + ":" + (lineNumber + 1)].ToString();
            Wabbitemu.Z80_State state = debugger.getState();
            Wabbitemu.MEMSTATE memState = debugger.getMemState();
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
            debugger.setMemState(memState);
            debugger.setState(state);
            debugPanel.updateRegisters();
        }

        public void addStaticLabels(TextEditorControl editorBox)
        {
            string[] editorText = editorBox.Text.ToUpper().Split('\n');
            string label;
            int location;
            int lineNum;
            string line;
            int commentChar;
            TextMarker newMarker;
            foreach (DictionaryEntry keyword in staticLabels)
            {
                lineNum = 0;
                label = keyword.Key.ToString();
                while (lineNum < editorText.Length)
                {
                    location = 0;
                    commentChar = editorText[lineNum].IndexOf(';');
                    line = editorText[lineNum].Contains(";")
                               ? editorText[lineNum].Remove(commentChar, editorText[lineNum].Length - commentChar)
                               : editorText[lineNum];
                    if (line.Length == 0 || !char.IsWhiteSpace(line[0]) || line.Contains("\""))
                    {
                        lineNum++;
                        continue;
                    }

                    while (location < line.Length && Char.IsWhiteSpace(line[location]))
                        location++;
                    while (location < line.Length && Char.IsLetter(line[location]))
                        location++;
                    if (line.IndexOf(label, location) != -1)
                    {
                        newMarker = new TextMarker(editorBox.Document.GetOffsetForLineNumber(lineNum) + line.IndexOf(label, location), label.Length, TextMarkerType.Invisible) { ToolTip = keyword.Value.ToString() };
                        staticLabelMarkers.Add(newMarker);
                        editorBox.Document.MarkerStrategy.AddMarker(newMarker);
                    }
                    lineNum++;
                }
            }
        }

        public void addBreakpoint(int lineNumber, string fileName)
        {
            if (!debugging)
                return;
            debugger.getState();
            if (!debugger.isWabbitOpen)
            {
                wabbitClosed();
                return;
            }
            Wabbitemu.breakpoint newBreakpoint = new Wabbitemu.breakpoint();
            string breakLocInfo = debugTableReverse[fileName + ":" + (lineNumber + 1)].ToString();
            newBreakpoint.Address = UInt16.Parse(breakLocInfo.Substring(3, 4), NumberStyles.HexNumber);
            newBreakpoint.Page = byte.Parse(breakLocInfo.Substring(0, 2), NumberStyles.HexNumber);
            if (isAnApp)
                newBreakpoint.Page = (byte)(apppage - newBreakpoint.Page);
            newBreakpoint.IsRam = newBreakpoint.Address < 0x8000 ? 0 : 1;
            breakpoints.Add(newBreakpoint);
            debugger.setBreakpoint(Handle, newBreakpoint);
            if (!debugger.isWabbitOpen)
                wabbitClosed();
        }

        public bool breakpointExists(int lineNumber, string fileName)
        {
            if (debugTableReverse != null)
                return debugTableReverse.ContainsKey(fileName + ":" + (lineNumber + 1));
            return true;
        }

        public void removeBreakpoint(int lineNumber, string fileName)
        {
            if (!debugging)
                return;
            debugger.getState();
            if (!debugger.isWabbitOpen)
            {
                wabbitClosed();
                return;
            }
            Wabbitemu.breakpoint newBreakpoint = new Wabbitemu.breakpoint();
            if (debugTableReverse.Contains(fileName + ":" + (lineNumber + 1)))
            {
                string breakLocInfo = debugTableReverse[fileName + ":" + (lineNumber + 1)].ToString();
                newBreakpoint.Address = UInt16.Parse(breakLocInfo.Substring(3, 4), NumberStyles.HexNumber);
                newBreakpoint.Page = (byte)(apppage - byte.Parse(breakLocInfo.Substring(0, 2), NumberStyles.HexNumber));
                newBreakpoint.IsRam = newBreakpoint.Address < 0x8000 ? 0 : 1;
                debugger.clearBreakpoint(newBreakpoint);
                breakpoints.Remove(newBreakpoint);
            }
        }

        private const int WM_USER = 0x0400;
        //[System.Security.Permissions.PermissionSet(System.Security.Permissions.SecurityAction.Demand, Name = "FullTrust")]
        protected override void WndProc(ref Message m)
        {
            // Listen for operating system messages.
            switch (m.Msg)
            {
                case WM_USER:
                    debugger.getState();
                    if (!debugger.isWabbitOpen)
                    {
                        wabbitClosed();
                        return;
                    }
                    trackWindow.updateVars();
                    debugPanel.updateFlags();
                    debugPanel.updateRegisters();
                    debugPanel.updateScreen();
                    string pagenum = getPageNum(debugger.getState().PC.ToString("X"));
                    string locInfo =
                debugTable[pagenum + ":" + debugger.getState().PC.ToString("X")
                    ].ToString();
                    string file = locInfo.Substring(0, locInfo.LastIndexOf(':'));
                    string lineNumber = locInfo.Substring(locInfo.LastIndexOf(':') + 1,
                                                          locInfo.Length - locInfo.LastIndexOf(':') - 1);
                    gotoLine(file, Convert.ToInt32(lineNumber));
                    highlightLine(Convert.ToInt32(lineNumber));
                    /*if (ActiveMdiChild != null)
                        if (((newEditor)ActiveMdiChild).ToolTipText != file)
                        {
                            bool isOpen = false;
                            foreach (IDockContent content in dockPanel.Documents)
                            {
                                if (content.DockHandler.ToolTipText == file)
                                {
                                    var doc = (newEditor)content;
                                    doc.Show();
                                    isOpen = true;
                                }
                            }
                            if (!isOpen)
                            {
                                newEditor doc = new newEditor {Text = file, TabText = file};
                                doc.Show(dockPanel);
                                doc.openFile(file);
                            }
                        }*/
                    // debugger.clearBreakpoint(false, (int)m.WParam, (int)m.LParam);
                    break;
            }
            base.WndProc(ref m);
        }

        private void removeHighlight()
        {
            if (ActiveMdiChild == null)
                return;
            foreach (newEditor child in MdiChildren)
                if (child.editorBox.FileName == highlight.Tag)
                {
                    child.Show();
                    break;
                }
            ((newEditor)ActiveMdiChild).editorBox.Document.MarkerStrategy.RemoveMarker(highlight);
        }

        private void stepButton_Click(object sender, EventArgs e)
        {
            debugger.getState();
            if (!debugger.isWabbitOpen)
                wabbitClosed();
            else
                step(debugTable);
        }

        private void step(Hashtable debugTable)
        {
            debugger.getState();
            if (!debugger.isWabbitOpen)
            {
                wabbitClosed();
                return;
            }
            removeHighlight();
            //need to clear the old breakpoint so lets save it
            string locInfo =
                debugTable[getPageNum(debugger.getState().PC.ToString("X")) + ":" + debugger.getState().PC.ToString("X")].ToString();
            string file = locInfo.Substring(0, locInfo.LastIndexOf(':'));
            string lineNumber = locInfo.Substring(locInfo.LastIndexOf(':') + 1,
                                                  locInfo.Length - locInfo.LastIndexOf(':') - 1);
            string newLineNumber = lineNumber;
            string newFile = file;
            string newLocInfo;
            string address;
            string page;
            Wabbitemu.Z80_State state;
            //int counter = 0;
            while (newLineNumber == lineNumber)
            {
                if (stepover)
                    debugger.stepOver();
                else
                    debugger.step();
                state = debugger.getState();
                page = getPageNum(state.PC.ToString("X"));
                address = state.PC.ToString("X4");
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
            gotoLine(newFile, Convert.ToInt32(newLineNumber));
            /*if (newFile != file)
            {
                bool isOpen = false;
                foreach (IDockContent content in dockPanel.Documents)
                {
                    if (content.DockHandler.ToolTipText == newFile)
                    {
                        var doc = (newEditor) content;
                        doc.Show();
                        isOpen = true;
                    }
                }
                if (!isOpen)
                {
                    var doc = new newEditor();
                    doc.Text = newFile;
                    doc.TabText = newFile;
                    doc.Show(dockPanel);
                    doc.openFile(newFile);
                }
            }*/
            highlightLine(Convert.ToInt32(newLineNumber));
            trackWindow.updateVars();
            debugPanel.updateFlags();
            debugPanel.updateRegisters();
            debugPanel.updateScreen();
        }

        private void highlightLine(int newLineNumber)
        {
            //this code highlights the current line
            //I KNOW IT WORKS DONT FUCK WITH IT
            TextEditorControl editorBox = ((newEditor)(ActiveMdiChild)).editorBox;
            TextArea textArea = editorBox.ActiveTextAreaControl.TextArea;
            editorBox.ActiveTextAreaControl.ScrollTo(newLineNumber - 1);
            editorBox.ActiveTextAreaControl.Caret.Line = newLineNumber - 1;
            int start = textArea.Caret.Offset;
            int length = editorBox.Document.TextContent.Split('\n')[textArea.Caret.Line].Length;
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
            highlight = new TextMarker(start, length, TextMarkerType.SolidBlock, Color.Yellow, Color.Black);
            highlight.Tag = editorBox.FileName;
            editorBox.Document.MarkerStrategy.AddMarker(highlight);
            editorBox.Refresh();
        }

        private string getPageNum(string address)
        {
            int addressLoc = int.Parse(address, NumberStyles.HexNumber);
            string page = "00";
            Wabbitemu.MEMSTATE memstate = debugger.getMemState();
            if (addressLoc < 0x4000) // && memstate.ram0 == 0)
                page = memstate.page0.ToString("X2");
            if (addressLoc >= 0x4000 && addressLoc < 0x8000) // && memstate.ram1 == 0)
                page = ((int)apppage - memstate.page1).ToString("X2");
            if (addressLoc >= 0x8000 && addressLoc < 0xC000) // && memstate.ram2 == 0)
                page = memstate.page2.ToString("X2");
            if (addressLoc >= 0xC000 && addressLoc < 0x10000) // && memstate.ram3 == 0)
                page = memstate.page3.ToString("X2");
            if (!debugger.isWabbitOpen)
                wabbitClosed();
            return page;
        }

        private int getRamState(string address)
        {
            int addressLoc = int.Parse(address, NumberStyles.HexNumber);
            int isRam = 0;
            Wabbitemu.MEMSTATE memstate = debugger.getMemState();
            if (addressLoc < 0x4000) // && memstate.ram0 == 0)
                isRam = memstate.ram0;
            if (addressLoc >= 0x4000 && addressLoc < 0x8000) // && memstate.ram1 == 0)
                isRam = memstate.ram1;
            if (addressLoc >= 0x8000 && addressLoc < 0xC000) // && memstate.ram2 == 0)
                isRam = memstate.ram2;
            if (addressLoc >= 0xC000 && addressLoc < 0x10000) // && memstate.ram3 == 0)
                isRam = memstate.ram3;
            if (!debugger.isWabbitOpen)
                wabbitClosed();
            return isRam;
        }

        private Hashtable staticLabels;
        private List<TextMarker> staticLabelMarkers;
        private void parseSymFile(string symFileContents)
        {
            staticLabels = new Hashtable();
            string[] lines = symFileContents.Split('\n');
            int equalsLoc;
            foreach (string line in lines)
            {
                if (line != "")
                {
                    equalsLoc = line.IndexOf('=');
                    staticLabels.Add(line.Substring(0, equalsLoc - 1),
                                     line.Substring(equalsLoc + 2, line.Length - equalsLoc - 2));
                }
            }
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
                    if (projectOpen)
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
                        debugginz.Add(temp, currentFile + ':' + line.Substring(0, 5).Trim());
                    if (!reverseDebugginz.ContainsKey(currentFile + ':' + line.Substring(0, 5).Trim()))
                        reverseDebugginz.Add(currentFile + ':' + line.Substring(0, 5).Trim(), temp);
                }
            }
        }
    }
}
