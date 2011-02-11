using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Classes;
#if USE_DLL
using SPASM;
#endif
//using Revsoft.Wabbitcode.Extensions;

namespace Revsoft.Wabbitcode.Services
{
	public static class AssemblerService
	{
		public static List<Errors> ErrorsInFiles { get; set; }
		const string quote = "\"";
		public static bool AssembleFile(string filePath, string assembledName, bool sendFileEmu)
		{
			Resources.GetResource("spasm.exe", FileLocations.SpasmFile);
			//Clear any other assemblings
            //this needs to be done when the button is pressed, as the project build system uses this function
            //DockingService.OutputWindow.ClearOutput();
			//Get our emulator
			Resources.GetResource("Wabbitemu.exe", FileLocations.WabbitemuFile);
#if !USE_DLL
			//create two new processes to run
			//setup wabbitspasm to run silently
			Process wabbitspasm = new Process
			{
				StartInfo =
				{
					FileName = FileLocations.SpasmFile,
					RedirectStandardOutput = true,
					RedirectStandardError = true,
					UseShellExecute = false,
					CreateNoWindow = true
				}
			};

			//some strings we'll need to build 
			string originalDir = ProjectService.IsInternal ? Path.GetDirectoryName(filePath) : ProjectService.ProjectDirectory;
			string includedir = "-I \"" + Application.StartupPath + "\"";
			if (Properties.Settings.Default.includeDir != "" || !ProjectService.IsInternal)
			{
				List<string> dirs = ProjectService.IsInternal ? 
							Properties.Settings.Default.includeDir.Split('\n').ToList<string>() :
							ProjectService.Project.IncludeDir;
				foreach (string dir in dirs)
					if (dir != "")
						includedir += ";\"" + dir + "\"";
			}
			string fileName = Path.GetFileName(filePath);
            string caseSensitive = Properties.Settings.Default.caseSensitive ? " -A " : " ";
			wabbitspasm.StartInfo.Arguments = includedir + caseSensitive + "-T -L " + quote + filePath + quote + " " + quote + assembledName + quote;
			wabbitspasm.StartInfo.WorkingDirectory = originalDir;
			wabbitspasm.Start();
#else
            AssemblerClass Assembler = new AssemblerClass();
            string originalDir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            //ShowMessage();
            Assembler.ClearIncludeDirectories();
            Assembler.ClearDefines();
            Assembler.AddIncludeDirectory(originalDir);
            //if the user has some include directories we need to format them
            if (Properties.Settings.Default.includeDir != "")
            {
                string[] dirs = Properties.Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                    if (dir != "")
                        Assembler.AddIncludeDirectory(dir);
            }
            //now we can set the args for spasm
            IStream pStream = Assembler.GetOutputStream();
            CStreamWrapper TestStream = new CStreamWrapper(pStream);
            StreamReader sr = new StreamReader(TestStream);

            Assembler.SetInputFile(filePath);
            Assembler.SetOutputFile(assembledName);

            Assembler.SetFlags(SPASM.AssemblyFlags.MODE_NORMAL | AssemblyFlags.MODE_LIST);
            //assemble that fucker
            Assembler.Assemble();

            StringBuilder builder = new StringBuilder();
            string line;
            do
            {
                line = sr.ReadLine();
                builder.Append(line);
                builder.Append("\n");
            } while (line != null);
                       
#endif
			//lets write it to the output window so the user knows whats happening
            string outputText = "==================" + Path.GetFileName(filePath) + "==================\r\n" +
                                "Assembling " + filePath + "\r\n" +
#if USE_DLL
                                builder.ToString();
#else
                                wabbitspasm.StandardOutput.ReadToEnd();
#endif
			bool errors = outputText.Contains("error");
			showPanelDelegate showPanels = new showPanelDelegate(ShowErrorPanels);
			DockingService.MainForm.Invoke(showPanels, new[] { outputText, originalDir });
			if (errors)
			{
				
			}
			//we need to check for errors
			//if (Settings.Default.sendFileEmu && sendFileEmu && !errors)
				//SendFileEmu(assembledName);
			//tell if the assembly was successful
			return !errors;
		}

		private delegate void showPanelDelegate(string text, string originaldir);
		private static void ShowErrorPanels(string outputText, string originaldir)
		{
#if !DEBUG
			try
			{
#endif
                DockingService.OutputWindow.SetText(outputText);
				DockingService.OutputWindow.HighlightOutput();
				//its more fun with colors
				DockingService.ErrorList.ParseOutput(outputText, originaldir);
				DockingService.ShowDockPanel(DockingService.ErrorList);
				DockingService.ShowDockPanel(DockingService.OutputWindow);
                if (DockingService.ActiveDocument != null)
                    DockingService.ActiveDocument.Refresh();
#if !DEBUG
            }
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
#endif
		}

		//static Thread assemblerThread;
		internal static void AssembleCurrentFile()
		{
            if (!ProjectService.IsInternal)
                ThreadPool.QueueUserWorkItem(new WaitCallback(AssembleProject));
            //assemblerThread = new Thread(AssembleProject);
            else if (DockingService.ActiveDocument != null)
            {
                bool saved = DockingService.ActiveDocument.SaveFile();
                if (saved)
                    ThreadPool.QueueUserWorkItem(new WaitCallback(AssembleFile));
                    //assemblerThread = new Thread(AssembleFile);
            }
            else
                return;
			//assemblerThread.Priority = ThreadPriority.BelowNormal;
			//assemblerThread.Start();
		}

		private static void AssembleFile(object data)
		{
			string text = DockingService.ActiveDocument.FileName;
			AssembleFile(text, Path.ChangeExtension(text, GetExtension(Properties.Settings.Default.outputFile)), true);			
		}

        private static bool isBuildProj;
        public static bool IsBuildingProject
        {
            get { return isBuildProj; }
        }
		public static void AssembleProject(object data)
		{
            isBuildProj = true;
            DockingService.OutputWindow.ClearOutput();
			ProjectService.Project.BuildSystem.Build();
            isBuildProj = false;
		}

		public static bool CreateSymTable(string filePath, string assembledName)
		{
			Resources.GetResource("spasm.exe", FileLocations.SpasmFile);
			//Clear any other assemblings
            DockingService.OutputWindow.ClearOutput();
			//Get our emulator
			Resources.GetResource("Wabbitemu.exe", FileLocations.WabbitemuFile);
#if !USE_DLL
			//create two new processes to run
			Process wabbitspasm = new Process
			{
				StartInfo =
				{
					FileName = FileLocations.SpasmFile,
					RedirectStandardOutput = true,
					RedirectStandardError = true,
					UseShellExecute = false,
					CreateNoWindow = true
				}
			};
			//setup wabbitspasm to run silently

			//some strings we'll need to build 
			string originalDir = Path.GetDirectoryName(filePath);
			string includedir = "-I \"" + Application.StartupPath + "\"";
			if (Properties.Settings.Default.includeDir != "")
			{
				string[] dirs = Properties.Settings.Default.includeDir.Split('\n');
				foreach (string dir in dirs)
				{
					if (dir != "")
						includedir += ";\"" + dir + "\"";
				}
			}
			string fileName = Path.GetFileName(filePath);
			// filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
			//string assembledName = Path.ChangeExtension(fileName, outputFileExt);
			wabbitspasm.StartInfo.Arguments = includedir + " -L " + fileName + " " + quote + assembledName + quote;
			wabbitspasm.StartInfo.WorkingDirectory = originalDir;
			wabbitspasm.Start();

#else
            string originalDir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            SpasmMethods.ClearIncludes();
            SpasmMethods.ClearDefines();
            SpasmMethods.AddInclude(originalDir);
            //if the user has some include directories we need to format them
            if (Properties.Settings.Default.includeDir != "")
            {
                string[] dirs = Properties.Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                {
                    if (dir != "")
                        SpasmMethods.AddInclude(dir);
                }
            }
            //get the file name we'll use and use it to create the assembled name
            string fileName = filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //string assembledName = Path.ChangeExtension(fileName, outputFileExt);
            //now we can set the args for spasm
            int error = 0;
            error |= SpasmMethods.SetInputFile(Path.Combine(originalDir, fileName));
            error |= SpasmMethods.SetOutputFile(Path.Combine(originalDir, assembledName));
            //emulator setup
            //emulator.StartInfo.FileName = emuLoc;
            //assemble that fucker
            uint STD_ERROR_HANDLE = 0xfffffff4;
            uint STD_OUTPUT_HANDLE = 0xfffffff5;
            StreamWriter test = new StreamWriter(Application.StartupPath + "\\test.txt");
            //FileStream test = new FileStream(Application.StartupPath + "\\test.txt", FileMode.Create);
            //SetStdHandle(STD_ERROR_HANDLE, test.Handle);
            //SetStdHandle(STD_OUTPUT_HANDLE, test.Handle);
            Console.SetOut(test);
            Console.SetError(test);
            //StreamReader reader = myprocess.StandardOutput;
            SpasmMethods.RunAssembly();
            Console.WriteLine("test line");
            //string tryread = reader.ReadToEnd();
            //string output = myprocess.StandardOutput.ReadToEnd();
            test.Flush();
            test.Close();
#endif
			DockingService.ShowDockPanel(DockingService.OutputWindow);
			//lets write it to the output window so the user knows whats happening
            string outputText = "==================" + fileName + "==================\r\n" +
                                                "SymTable for " + originalDir + "\\" + fileName + "\r\n"
#if USE_DLL
;
#else
                                                + wabbitspasm.StandardOutput.ReadToEnd();
#endif
			DockingService.OutputWindow.SetText(outputText);
			//its more fun with colors
			DockingService.OutputWindow.HighlightOutput();

			bool errors = outputText.Contains("error");
			if (errors)
				DockingService.ShowDockPanel(DockingService.OutputWindow);
			//its more fun with colors
			DockingService.ErrorList.ParseOutput(outputText, originalDir);
			if (errors)
				DockingService.ShowDockPanel(DockingService.ErrorList);
			//tell if the assembly was successful
			//if (error != 0)
			//    return false;
			//else
			//    return true;
			return !errors;
		}


		internal static string GetExtension(int outputFile)
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

        internal static void InitAssembler()
        {
            ErrorsInFiles = new List<Errors>();
        }
    }
}
