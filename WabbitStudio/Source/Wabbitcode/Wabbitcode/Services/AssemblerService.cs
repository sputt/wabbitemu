using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Classes;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using System.Threading;
using System.Xml;

namespace Revsoft.Wabbitcode.Services
{
	public static class AssemblerService
	{
		public static List<Errors> ErrorsInFiles { get; set; }
		const string quote = "\"";
		public static bool AssembleFile(string filePath, string assembledName, bool sendFileEmu)
		{
			Resources.GetResource("spasm.exe", "spasm.exe");
			//Clear any other assemblings
			//outputWindow.outputWindowBox.Text = "";
			//Get our emulator
			Resources.GetResource("Wabbitemu.exe", "Wabbitemu.exe");
#if USE_DLL == false
			//create two new processes to run
			//setup wabbitspasm to run silently
			Process wabbitspasm = new Process
			{
				StartInfo =
				{
					FileName = Path.Combine(Application.StartupPath, "spasm.exe"),
					RedirectStandardOutput = true,
					RedirectStandardError = true,
					UseShellExecute = false,
					CreateNoWindow = true
				}
			};

			//some strings we'll need to build 
			string originaldir = ProjectService.IsInternal ? Path.GetDirectoryName(filePath) : ProjectService.ProjectDirectory;
			string includedir = "-I \"" + Application.StartupPath + "\"";
			if (Properties.Settings.Default.includeDir != "" || !ProjectService.IsInternal)
			{
				List<string> dirs = ProjectService.IsInternal ? 
							Properties.Settings.Default.includeDir.Split('\n').ToList<string>() :
							ProjectService.Project.IncludeDir;
				foreach (string dir in dirs)
				{
					if (dir != "")
						includedir += ";\"" + dir + "\"";
				}
			}
			string fileName = Path.GetFileName(filePath);
			// filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
			//string assembledName = Path.ChangeExtension(fileName, outputFileExt);
			wabbitspasm.StartInfo.Arguments = includedir + " -T -L " + quote + filePath + quote + " " + quote + assembledName + quote;
			wabbitspasm.StartInfo.WorkingDirectory = originaldir;
			wabbitspasm.Start();
#else
            string originalDir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            //ShowMessage();
            ClearIncludes();
            ClearDefines();
            AddInclude(originalDir);
            //if the user has some include directories we need to format them
            if (Settings.Default.includeDir != "")
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                    if (dir != "")
                        AddInclude(dir);
            }
            //get the file name we'll use and use it to create the assembled name
            string fileName = filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //assembledName = Path.ChangeExtension(fileName, getExtension(fileName));
            //now we can set the args for spasm
            int error = 0;
            //SetMode((int)MODE.MODE_SYMTABLE);
            error |= SetInputFileA(Path.Combine(originalDir, fileName));
            error |= SetOutputFileA(Path.Combine(originalDir, assembledName));
            //emulator setup
            //emulator.StartInfo.FileName = emuLoc;
            //assemble that fucker
            uint STD_ERROR_HANDLE = 0xFFFFFFF4;
            uint STD_OUTPUT_HANDLE = 0xFFFFFFF5;

            StreamWriter test = new StreamWriter(Application.StartupPath + "\\test.txt");
            //FileStream test = new FileStream(Application.StartupPath + "\\test.txt", FileMode.Create);
            //SetStdHandle(STD_ERROR_HANDLE, test.Handle);
            //IntPtr test2 = GetStdHandle(STD_OUTPUT_HANDLE);
            //SetStdHandle(STD_OUTPUT_HANDLE, test.Handle);
            //test2 = GetStdHandle(STD_OUTPUT_HANDLE);
            //Console.SetOut(test);
            //Console.SetError(test);
            //StreamReader reader = myprocess.StandardOutput;
            //Console.WriteLine("test line1");
            try
            {
                RunAssembly();
            }
            catch (Exception)
            {

            }
            //Console.WriteLine("test line2");
            //string tryread = reader.ReadToEnd();
            //string output = myprocess.StandardOutput.ReadToEnd();
            //test2 = GetStdHandle(STD_OUTPUT_HANDLE);
            test.Flush();
            test.Close();            
#endif
			//lets write it to the output window so the user knows whats happening
			string outputText = wabbitspasm.StartInfo.Arguments + "\n==================" +
															fileName + "==================\r\n" +
															"Assembling " + originaldir + "\\" + fileName + "\r\n" +
#if USE_DLL == false
 															wabbitspasm.StandardOutput.ReadToEnd();
#else
															"";
#endif
			bool errors = outputText.Contains("error");
			showPanelDelegate showPanels = new showPanelDelegate(ShowErrorPanels);
			DockingService.MainForm.Invoke(showPanels, new[] { outputText, originaldir });
			if (errors)
			{
				
			}
			//we need to check for errors
			//if (Settings.Default.sendFileEmu && sendFileEmu && !errors)
				//SendFileEmu(assembledName);
			//tell if the assembly was successful
			//if (error != 0)
			//    return false;
			//else
			//    return true;
			return !errors;
		}

		private delegate void showPanelDelegate(string text, string originaldir);
		private static void ShowErrorPanels(string outputText, string originaldir)
		{
			DockingService.OutputWindow.SetText(outputText);
			DockingService.OutputWindow.HighlightOutput();
			//its more fun with colors
			DockingService.ErrorList.ParseOutput(outputText, originaldir);
			DockingService.ShowDockPanel(DockingService.ErrorList);
			DockingService.ShowDockPanel(DockingService.OutputWindow);
			if (DockingService.ActiveDocument != null)
				DockingService.ActiveDocument.Refresh();
		}

		static Thread assemblerThread;
		internal static void AssembleCurrentFile()
		{
			if (!ProjectService.IsInternal)
				assemblerThread = new Thread(AssembleProject);
			else if (DockingService.ActiveDocument != null)
			{
				bool saved = DockingService.ActiveDocument.SaveFile();
				if (saved)
					assemblerThread = new Thread(AssembleFile);
			}
			else
				return;
			assemblerThread.Priority = ThreadPriority.BelowNormal;
			assemblerThread.Start();
		}

		private static void AssembleFile()
		{
			string text = DockingService.ActiveDocument.FileName;
			AssembleFile(text, Path.ChangeExtension(text, GetExtension(Properties.Settings.Default.outputFile)), true);			
		}

		public static void AssembleProject()
		{
			ProjectService.Project.BuildSystem.Build();
		}

		public static bool CreateSymTable(string filePath, string assembledName)
		{
			Resources.GetResource("spasm.exe", "spasm.exe");
			//Clear any other assemblings
			//outputWindow.outputWindowBox.Text = "";
			//Get our emulator
			Resources.GetResource("Wabbitemu.exe", "Wabbitemu.exe");
#if USE_DLL == false
			//create two new processes to run
			Process wabbitspasm = new Process
			{
				StartInfo =
				{
					FileName = Path.Combine(Application.UserAppDataPath, "spasm.exe"),
					RedirectStandardOutput = true,
					RedirectStandardError = true,
					UseShellExecute = false,
					CreateNoWindow = true
				}
			};
			//setup wabbitspasm to run silently

			//some strings we'll need to build 
			string originaldir = Path.GetDirectoryName(filePath);// filePath.Substring(0, filePath.LastIndexOf('\\'));
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
			wabbitspasm.StartInfo.WorkingDirectory = originaldir;
			wabbitspasm.Start();

#else
            string originalDir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            ClearIncludes();
            ClearDefines();
            AddInclude(originalDir);
            //if the user has some include directories we need to format them
            if (Settings.Default.includeDir != "")
            {
                string[] dirs = Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                {
                    if (dir != "")
                        AddInclude(dir);
                }
            }
            //get the file name we'll use and use it to create the assembled name
            string fileName = filePath.Substring(filePath.LastIndexOf('\\') + 1, filePath.Length - filePath.LastIndexOf('\\') - 1);
            //string assembledName = Path.ChangeExtension(fileName, outputFileExt);
            //now we can set the args for spasm
            int error = 0;
            error |= SetInputFileA(Path.Combine(originalDir, fileName));
            error |= SetOutputFileA(Path.Combine(originalDir, assembledName));
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
            RunAssembly();
            Console.WriteLine("test line");
            //string tryread = reader.ReadToEnd();
            //string output = myprocess.StandardOutput.ReadToEnd();
            test.Flush();
            test.Close();
#endif
			DockingService.ShowDockPanel(DockingService.OutputWindow);
			//lets write it to the output window so the user knows whats happening
			string outputText = "==================" + fileName + "==================\r\n" +
												"SymTable for " + originaldir + "\\" + fileName + "\r\n" +
												wabbitspasm.StandardOutput.ReadToEnd();
			DockingService.OutputWindow.SetText(outputText);
			//its more fun with colors
			DockingService.OutputWindow.HighlightOutput();

			bool errors = outputText.Contains("error");
			if (errors)
				DockingService.ShowDockPanel(DockingService.OutputWindow);
			//its more fun with colors
			DockingService.ErrorList.ParseOutput(outputText, originaldir);
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
	}
}
