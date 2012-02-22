#define WABBITEMU
using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Net;
using System.Security.Principal;
using System.Security;
using System.Security.AccessControl;
using System.Security.Permissions;
using System.Collections.Generic;

namespace Revsoft.AutoUpdater
{
	class Program
	{
		static string processName = "";
		static void Main(string[] args)
		{
			List<string> filePaths = new List<string>(), downloadPaths = new List<string>();
			bool isElevated;
			WindowsPrincipal principal = new WindowsPrincipal(WindowsIdentity.GetCurrent());
			isElevated = principal.IsInRole(WindowsBuiltInRole.Administrator);
			bool requiresAdmin = false;
			for (int i = 0; i < args.Length; i++)
			{
				string arg = args[i];
				if (arg == "-A")
					if (!isElevated)
						requiresAdmin = true;
					else
						continue;
				else if (arg == "-R")
					processName = args[++i];
				else
				{
					requiresAdmin |= CheckNeedAdmin(arg);
					filePaths.Add(arg);
					downloadPaths.Add(args[++i]);
				}
			}
			if (requiresAdmin)
			{
				Process process = new Process()
				{
					StartInfo =
					{
						UseShellExecute = true,
						Verb = "runas",
						FileName = "Revsoft.AutoUpdater.exe",
						Arguments = "-A " + String.Join(" ", args)
					}
				};
				process.Start();
				return;
			}
			bool closedProcess = false;
			Process[] wabbitcode = Process.GetProcesses();
			foreach (Process openWabbit in wabbitcode) {
				foreach (string file in filePaths)
				{
					if (openWabbit.ProcessName == Path.GetFileName(file))
					{
						openWabbit.Close();
						closedProcess = true;
					}
				}
			}
			if (closedProcess)
				Thread.Sleep(5000);
			for (int i = 0; i < filePaths.Count; i++)
			{
				string filePath = filePaths[i];
				string downloadPath = downloadPaths[i];
				try
				{
					Console.WriteLine("Downloading " + Path.GetFileName(filePath));
					bool succeeded = DownloadTempFile(downloadPath, filePath);
					if (!succeeded)
					{
						Console.WriteLine("Error downloading temporary file");
						continue;
					}
					bool needsUpdate = NeedsUpdate(filePath);
					if (needsUpdate)
						Update(filePath);
					else
					{
						Console.WriteLine(Path.GetFileName(filePath) + " is up to date");
						File.Delete(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
													Path.GetFileName(filePath)));
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine(ex.ToString());
				}
			}
			try
			{
				if (!string.IsNullOrEmpty(processName))
				{
					Process process = new Process();
					process.StartInfo.FileName = processName;
					process.StartInfo.WorkingDirectory = Directory.GetCurrentDirectory();
					process.Start();
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error starting process" + processName + "\n" + ex);
			}
			Thread.Sleep(2000);
			
		}

		static bool CheckNeedAdmin(string filePath)
		{
			bool requiresAdmin = false;
			FileStream stream = null;
			try
			{
				string tempFile = Path.Combine(Path.GetDirectoryName(filePath), Path.GetRandomFileName());
				stream = File.Create(tempFile);
				if (stream != null)
				{
					requiresAdmin = false;
					stream.Close();
					File.Delete(tempFile);
				}
			}
			catch (Exception)
			{
				requiresAdmin = true;
			}
			finally
			{
				if (stream != null)
					stream.Close();
			}
			return requiresAdmin;
		}

		static bool DownloadTempFile(string downloadPath, string filePath)
		{
			bool succeeded = true;
			WebClient Client;
			try
			{
				string fileName = Path.GetFileName(filePath);
				string tempPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), fileName);
				Client = new WebClient();
				Client.DownloadFile(downloadPath, tempPath);
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.ToString());
				succeeded = false;
			}
			return succeeded;
		}

		static bool NeedsUpdate(string filePath)
		{
			string tempPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), Path.GetFileName(filePath));
			if (!File.Exists(filePath))
				return true;
			FileVersionInfo info = FileVersionInfo.GetVersionInfo(filePath);
			FileVersionInfo newInfo = FileVersionInfo.GetVersionInfo(tempPath);
			Console.WriteLine(info.FileVersion);
			Console.WriteLine(newInfo.FileVersion);
			if (info.FileMajorPart < newInfo.FileMajorPart)
				return true;
			if (info.FileMajorPart > newInfo.FileMajorPart)
				return false;
			if (info.FileMinorPart < newInfo.FileMinorPart)
				return true;
			if (info.FileMinorPart > newInfo.FileMinorPart)
				return false;
			if (info.FileBuildPart < newInfo.FileBuildPart)
				return true;
			if (info.FileBuildPart > newInfo.FileBuildPart)
				return false;
			if (info.FilePrivatePart < newInfo.FilePrivatePart)
				return true;
			return false;
		}

		static void Update(string filePath)
		{
			string fileName =  Path.GetFileName(filePath);
			string tempPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), fileName);
			File.Delete(filePath);
			File.Move(tempPath, filePath);
			Console.WriteLine(fileName + " successfully updated");
		}
	}
}
