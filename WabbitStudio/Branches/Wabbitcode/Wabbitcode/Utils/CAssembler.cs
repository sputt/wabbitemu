namespace Revsoft.Wabbitcode
{
    using System;
    using System.ComponentModel;
    using System.Diagnostics;
    using System.Drawing;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Threading;
    using System.Windows.Forms;
    using System.Xml;

    using Revsoft.Wabbitcode.Classes;
    using Revsoft.Wabbitcode.Properties;

    public class Assembler
    {
        private bool codeCounting;
        private bool CompletedAssembly;
        private string filePath, assembledName;
        private bool sendFile = false;

        public Assembler()
        {
            BackgroundWorker assembler = new BackgroundWorker();
            GlobalClass.mainForm.pipeServer.MessageReceived += GlobalClass.mainForm.pipeServer_MessageReceived;
            GlobalClass.mainForm.pipeServer.MessageReceived -= GlobalClass.mainForm.pipeServer_HandleCodeCount;
            assembler.DoWork += this.assembleProject;
            assembler.RunWorkerCompleted += this.doneAssembling;
            assembler.RunWorkerAsync();
            GlobalClass.outputWindow.outputWindowBox.Text = "==================" + GlobalClass.project.projectName + "==================\r\n" +
                    "Assembling " + GlobalClass.project.projectName + "\r\n";
        }

        /// <summary>
        /// Creates a new assembler
        /// </summary>
        /// <param name="text">The input for the project (wcodeproj file) or the lines to feed to the code counter</param>
        /// <param name="CodeCount">Whether we should count code for this assembling</param>
        public Assembler(string text, ASM_MODE CodeCount)
        {
            BackgroundWorker assembler;
            switch (CodeCount)
            {
            case ASM_MODE.MODE_CODE_COUNTER:
                GlobalClass.mainForm.pipeServer.MessageReceived -= GlobalClass.mainForm.pipeServer_MessageReceived;
                GlobalClass.mainForm.pipeServer.MessageReceived += GlobalClass.mainForm.pipeServer_HandleCodeCount;
                this.filePath = text;
                assembler = new BackgroundWorker();
                assembler.DoWork += this.countCode;
                assembler.RunWorkerAsync();
                break;
            case ASM_MODE.MODE_COMMANDLINE:
                GlobalClass.mainForm.commandlineOutput = "";
                GlobalClass.mainForm.pipeServer.MessageReceived -= GlobalClass.mainForm.pipeServer_MessageReceived;
                GlobalClass.mainForm.pipeServer.MessageReceived += GlobalClass.mainForm.pipeServer_HandleCommandline;
                this.filePath = text;
                assembler = new BackgroundWorker();
                assembler.DoWork += this.runCommandLine;
                assembler.RunWorkerAsync();
                break;
            case ASM_MODE.MODE_NORMAL:
                assembler = new BackgroundWorker();
                GlobalClass.mainForm.pipeServer.MessageReceived += GlobalClass.mainForm.pipeServer_MessageReceived;
                GlobalClass.mainForm.pipeServer.MessageReceived -= GlobalClass.mainForm.pipeServer_HandleCodeCount;
                assembler.DoWork += this.assembleCode;
                assembler.RunWorkerCompleted += this.doneAssembling;
                this.filePath = text;
                this.assembledName = Path.ChangeExtension(text, getExtension(Settings.Default.outputFile));
                assembler.RunWorkerAsync();
                GlobalClass.outputWindow.outputWindowBox.Text = "==================" + Path.GetFileName(this.filePath) + "==================\r\n" +
                        "Assembling " + Path.GetDirectoryName(this.filePath) + "\\" + Path.GetFileName(this.filePath) + "\r\n";
                break;
            }
        }

        public enum ASM_MODE
        {
            MODE_NORMAL = 1,
            MODE_CODE_COUNTER = 2,
            MODE_SYMTABLE = 4,
            MODE_STATS = 8,
            MODE_LIST = 16,
            MODE_COMMANDLINE = 32,
        }

        public bool Errors
        {
            get;
            set;
        }

        [DllImport("SPASM.dll", EntryPoint = "?AddDefine@@YGHPBD0@Z")]
        public static extern int AddDefine(
            [In, MarshalAs(UnmanagedType.LPStr)] string name,
            [In, MarshalAs(UnmanagedType.LPStr)] string value);

        [DllImport("SPASM.dll", EntryPoint = "?AddInclude@@YGHPBD@Z")]
        public static extern int AddInclude([In, MarshalAs(UnmanagedType.LPStr)] string directory);

        [DllImport("SPASM.dll", EntryPoint = "?ClearDefines@@YGHXZ")]
        public static extern int ClearDefines();

        [DllImport("SPASM.dll", EntryPoint = "?ClearIncludes@@YGHXZ")]
        public static extern int ClearIncludes();

        [DllImport("SPASM.dll", EntryPoint = "?RunAssembly@@YGHXZ")]
        public static extern int RunAssembly();

        // we done care about lpResult, make sure it is always NULL to avoid mem leak
        [DllImport("SPASM.dll", EntryPoint = "?RunAssemblyWithArguments@@YGHPADPAEH@Z")]
        public static extern int RunAssemblyWithArguments([In, MarshalAs(UnmanagedType.LPStr)]
                string szCommand, IntPtr lpResult, IntPtr cbResult);

        [DllImport("SPASM.dll", EntryPoint = "?SetInputFile@@YGHPBD@Z")]
        public static extern int SetInputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport("SPASM.dll", EntryPoint = "?SetOutputFile@@YGHPBD@Z")]
        public static extern int SetOutputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport("SPASM.dll", EntryPoint = "?ShowMessage@@YGHXZ")]
        public static extern int ShowMessage();

        public void Join()
        {
            while (!this.CompletedAssembly)
                { }
        }

        protected internal static string getExtension(int outputFile)
        {
            string outputFileExt = "bin";
            switch (outputFile)
            {
            case 1:
                outputFileExt = "73p";
                break;
            case 2:
                outputFileExt = "82p";
                break;
            case 3:
                outputFileExt = "83p";
                break;
            case 4:
                outputFileExt = "8xp";
                break;
            case 5:
                outputFileExt = "8xk";
                break;
            case 6:
                outputFileExt = "85p";
                break;
            case 7:
                outputFileExt = "85s";
                break;
            case 8:
                outputFileExt = "86p";
                break;
            case 9:
                outputFileExt = "86s";
                break;
            }

            return outputFileExt;
        }

        [DllImport("SPASM.dll", EntryPoint = "?SetMode@@YGXH@Z")]
        private static extern int SetMode(ASM_MODE mode);

        private void assembleCode(object sender, DoWorkEventArgs e)
        {
            string originalDir = Path.GetDirectoryName(this.filePath);
            ClearIncludes();
            ClearDefines();
            AddInclude(originalDir);

            // if the user has some include directories we need to format them
            if (Settings.Default.includeDir != "")
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                {
                    if (dir != "")
                    }
                {
                    AddInclude(dir);
                }
            }

            // get the file name we'll use and use it to create the assembled name
            string fileName = Path.GetFileName(this.filePath); // .Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            // now we can set the args for spasm
            int error = 0;

            // SetMode((int)MODE.MODE_SYMTABLE);
            error |= SetInputFile(Path.Combine(originalDir, fileName));
            error |= SetOutputFile(Path.Combine(originalDir, this.assembledName));

            // assemble that fucker
            try
            {
                RunAssembly();
            }
            catch (Exception)
                { }
            this.CompletedAssembly = true;
        }

        private void assembleProject(object sender, DoWorkEventArgs e)
        {
            XmlNodeList buildConfigs = GlobalClass.project.getBuildConfigs();

            // some strings we'll need to build
            string sendToEmu = "";
            XmlNodeList steps = buildConfigs[Settings.Default.buildConfig].ChildNodes;
            ClearIncludes();
            ClearDefines();
            AddInclude(GlobalClass.project.projectLoc);

            // if the user has some include directories we need to format them
            if (Properties.Settings.Default.includeDir != "")
            {
                string[] dirs = GlobalClass.project.getIncludeDirs();
                foreach (string dir in dirs)
                {
                    if (dir != "")
                    }
                {
                    AddInclude(dir);
                }
            }

            int counter = 1;
            foreach (XmlElement step in steps)
            {
                int error = 0;
                string fileName;
                string originalDir = Path.GetDirectoryName(step.InnerText);
                string assembledName;
                switch (step.Attributes["action"].Value[0])
                {
                    // normal assemble
                case 'C':
                    // try
                    // {
                    this.filePath = step.InnerText;
                    fileName = Path.GetFileName(this.filePath);
                    assembledName = Path.Combine(originalDir, GlobalClass.project.projectName + "." + getExtension(Convert.ToInt32(step.Attributes["type"].Value)));

                    // now we can set the args for spasm
                    error |= SetInputFile(this.filePath);
                    error |= SetOutputFile(assembledName);
                    error |= RunAssembly();
                    sendToEmu = assembledName;

                    // }
                    // catch (Exception ex)
                    // {
                    //    MessageBox.Show("Error: " + ex);
                    // }
                    counter++;
                    break;

                    // symbol table file
                case 'T':
                    try
                    {
                        fileName = Path.GetFileName(step.InnerText);
                        assembledName = Path.ChangeExtension(fileName, ".inc");
                        error |= SetInputFile(Path.Combine(originalDir, fileName));
                        error |= SetOutputFile(Path.Combine(originalDir, assembledName));
                        RunAssembly();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Error: " + ex);
                    }

                    counter++;
                    break;

                    // listing file
                case 'L':
                    this.filePath = step.InnerText;
                    assembledName = Path.Combine(originalDir, GlobalClass.project.projectName + ".lst");
                    counter++;
                    break;

                    // run external command
                case 'E':
                    try
                    {
                        Process cmd = new Process
                        {
                            StartInfo =
                            {
                                FileName = Path.Combine(GlobalClass.project.projectLoc, step.InnerText),
                                WorkingDirectory = GlobalClass.project.projectLoc,
                                UseShellExecute = false,
                                CreateNoWindow = true,
                                RedirectStandardOutput = true,
                                RedirectStandardError = true
                            }
                        };
                        cmd.Start();
                    }
                    catch (Exception ex)
                    {
                        // errorConsoleBox.Text += ex.ToString() + '\n';
                        MessageBox.Show("Error: " + ex);
                    }

                    counter++;
                    break;
                }
            }

            this.CompletedAssembly = true;
        }

        private void countCode(object sender, DoWorkEventArgs e)
        {
            SetMode(ASM_MODE.MODE_CODE_COUNTER);
            try
            {
                RunAssemblyWithArguments(this.filePath, IntPtr.Zero, IntPtr.Zero);
            }
            catch (Exception)
                { }
        }

        private void doneAssembling(object sender, RunWorkerCompletedEventArgs e)
        {
            // lets write it to the output window so the user knows whats happening
            GlobalClass.outputWindow.highlightOutput();
            GlobalClass.outputWindow.outputWindowBox.SelectionStart = GlobalClass.outputWindow.outputWindowBox.Text.Length;
            GlobalClass.outputWindow.outputWindowBox.ScrollToCaret();
            string outputText = GlobalClass.outputWindow.outputWindowBox.Text;
            bool errors = outputText.Contains("error");
            if (errors)
            {
                GlobalClass.outputWindow.Show(GlobalClass.mainForm.dockPanel);
            }

            // its more fun with colors
            GlobalClass.errorList.parseOutput(outputText, Path.GetDirectoryName(this.filePath));
            if (errors)
            {
                GlobalClass.errorList.Show(GlobalClass.mainForm.dockPanel);
            }

            // we need to check for errors
            this.Errors = errors;

            // if (Settings.Default.sendFileEmu && sendFile && !errors)
            //    sendFileEmu(assembledName);
        }

        private void runCommandLine(object sender, DoWorkEventArgs e)
        {
            SetMode(ASM_MODE.MODE_COMMANDLINE);
            try
            {
                RunAssemblyWithArguments(this.filePath, IntPtr.Zero, IntPtr.Zero);
            }
            catch (Exception)
                { }
        }
    }
}