using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Classes;
#if USE_ATL
using SPASM;
#endif
//using Revsoft.Wabbitcode.Extensions;

namespace Revsoft.Wabbitcode.Services
{
	public static class AssemblerService
	{
		public static readonly List<Error> ErrorsInFiles = new List<Error>();
		const string quote = "\"";
		public static bool AssembleFile(string filePath, string assembledName, bool silent)
		{
#if !USE_DLL
			//create two new processes to run
			//setup wabbitspasm to run silently
			var wabbitspasm = new Process
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
			if (!string.IsNullOrEmpty(Properties.Settings.Default.includeDir) || !ProjectService.IsInternal)
			{
				List<string> dirs = ProjectService.IsInternal ? 
							Properties.Settings.Default.includeDir.Split('\n').ToList<string>() :
							ProjectService.Project.IncludeDir;
				foreach (string dir in dirs)
					if (!string.IsNullOrEmpty(dir))
						includedir += ";\"" + dir + "\"";
			}
			string fileName = Path.GetFileName(filePath);
            string caseSensitive = Properties.Settings.Default.caseSensitive ? " -A " : " ";
			wabbitspasm.StartInfo.Arguments = includedir + caseSensitive + "-T -L " + quote + filePath + quote + " " + quote + assembledName + quote;
			wabbitspasm.StartInfo.WorkingDirectory = originalDir;
			wabbitspasm.Start();
#elif USE_ATL
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
			ParseOutput(outputText, originalDir);
			if (!silent)
			{
				showPanelDelegate showPanels = new showPanelDelegate(ShowErrorPanels);
				DockingService.MainForm.Invoke(showPanels, outputText, originalDir);
			}
			//tell if the assembly was successful
			return !errors;
		}

		public static void ParseOutput(string outputText, string startDir)
		{
			AssemblerService.ErrorsInFiles.Clear();
			string[] lines = outputText.Split('\n');
			foreach (string line in lines)
			{
				int thirdColon, secondColon, firstColon;
				string file, lineNum, description;
				if (line.Contains("error"))
				{
					firstColon = line.IndexOf(':', 3);
					secondColon = line.IndexOf(':', firstColon + 1);
					thirdColon = line.IndexOf(':', secondColon + 1);
					if (firstColon < 0 || secondColon < 0 || thirdColon < 0)
					{
						AssemblerService.ErrorsInFiles[AssemblerService.ErrorsInFiles.Count - 1].description += line;
					}
					else
					{
						file = Path.Combine(startDir, line.Substring(0, firstColon));
						lineNum = line.Substring(firstColon + 1, secondColon - firstColon - 1);
						int lineNumber;
						if (!int.TryParse(lineNum, out lineNumber))
							lineNumber = -1;
						description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
						ErrorsInFiles.Add(new Error(file, lineNumber, description, false));
					}
				}
				if (!line.Contains("warning"))
					continue;
				firstColon = line.IndexOf(':', 3);
				secondColon = line.IndexOf(':', firstColon + 1);
				thirdColon = line.IndexOf(':', secondColon + 1);
				if (firstColon < 0 || secondColon < 0 || thirdColon < 0)
				{
					ErrorsInFiles[ErrorsInFiles.Count - 1].description += line;
				}
				else
				{
					file = Path.Combine(startDir, line.Substring(0, firstColon));
					lineNum = line.Substring(firstColon + 1, secondColon - firstColon - 1);
					description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
					ErrorsInFiles.Add(new Error(file, Convert.ToInt32(lineNum), description, true));
				}
			}
		}

		private delegate void showPanelDelegate(string outputText, string originalDir);
		private static void ShowErrorPanels(string outputText, string originalDir)
		{
			try
			{
                DockingService.OutputWindow.SetText(outputText);
				DockingService.OutputWindow.HighlightOutput();
				//its more fun with colors
				DockingService.ErrorList.ParseOutput();
				DockingService.ShowDockPanel(DockingService.ErrorList);
				DockingService.ShowDockPanel(DockingService.OutputWindow);
                if (DockingService.ActiveDocument != null)
                    DockingService.ActiveDocument.Refresh();
				foreach (NewEditor child in DockingService.Documents)
					child.UpdateIcons();
            }
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
		}

		//static Thread assemblerThread;
		internal static void AssembleCurrentFile()
		{
            if (!ProjectService.IsInternal)
                ThreadPool.QueueUserWorkItem(new WaitCallback(AssembleProject));
            else if (DockingService.ActiveDocument != null)
            {
                bool saved = DockingService.ActiveDocument.SaveFile();
                if (saved)
                    ThreadPool.QueueUserWorkItem(new WaitCallback(AssembleFile));
            }
            else return;
		}

		private static void AssembleFile(object data)
		{
			string text = DockingService.ActiveDocument.FileName;
			AssembleFile(text, Path.ChangeExtension(text, GetExtension(Properties.Settings.Default.outputFile)), false);
		}

        private static bool isBuildProj;
        public static bool IsBuildingProject
        {
            get { return isBuildProj; }
        }
		public static void AssembleProject(object data)
		{
			bool silent = false;
			if (data != null)
				silent = (bool)data;
            isBuildProj = true;
			if (!silent)
				DockingService.OutputWindow.ClearOutput();
			ProjectService.Project.BuildSystem.Build(silent);
            isBuildProj = false;
		}

		public static bool CreateSymTable(string filePath, string assembledName, bool silent)
		{
			Resources.GetResource("spasm.exe", FileLocations.SpasmFile);
			//Clear any other assemblings
            DockingService.OutputWindow.ClearOutput();
			//Get our emulator
			Resources.GetResource("Wabbitemu.exe", FileLocations.WabbitemuFile);
#if !USE_DLL
			//create two new processes to run
			var wabbitspasm = new Process
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
			if (!string.IsNullOrEmpty(Properties.Settings.Default.includeDir))
			{
				string[] dirs = Properties.Settings.Default.includeDir.Split('\n');
				foreach (string dir in dirs)
				{
					if (!string.IsNullOrEmpty(dir))
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
			bool errors = outputText.Contains("error");
			ParseOutput(outputText, originalDir);
			if (!silent)
			{
				showPanelDelegate showPanels = new showPanelDelegate(ShowErrorPanels);
				DockingService.MainForm.Invoke(showPanels, outputText, originalDir);
			}
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
            
        }
    }
}
