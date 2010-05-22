using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Windows.Forms;
using System.Net;

namespace Revsoft.AutoUpdater
{
	class Program
	{
		static void Main(string[] args)
		{
			if (args.Length > 0 && string.IsNullOrEmpty(args[0]))
			{
				Process[] wabbitcode = Process.GetProcesses();
				foreach (Process openWabbit in wabbitcode)
					if (openWabbit.ProcessName.ToLower().Contains("wabbitcode"))
						openWabbit.Close();
			}
			Thread.Sleep(5000);
			WebClient Client = new WebClient();
			try
			{
				Console.WriteLine("Downloading Revsoft.Wabbitcode.exe");
				Client.DownloadFile("http://group.revsoft.org/Wabbitcode/Revsoft.Wabbitcode.exe", Path.Combine(Application.UserAppDataPath, "Revsoft.Wabbitcode.exe"));
				File.Delete("Revsoft.Wabbitcode.exe");
				File.Move(Path.Combine(Application.UserAppDataPath, "Revsoft.Wabbitcode.exe"), "Revsoft.Wabbitcode.exe");
            }
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
            try
			{
				Console.WriteLine("Downloading Revsoft.TextEditor.dll");
				Client.DownloadFile("http://group.revsoft.org/Wabbitcode/Revsoft.TextEditor.dll", Path.Combine(Application.UserAppDataPath, "Revsoft.TextEditor.dll"));
				File.Delete("Revsoft.TextEditor.dll");
				File.Move(Path.Combine(Application.UserAppDataPath, "Revsoft.TextEditor.dll"), "Revsoft.TextEditor.dll");
            }
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
            try
			{
				Console.WriteLine("Downloading Revsoft.Docking.dll");
				Client.DownloadFile("http://group.revsoft.org/Wabbitcode/Revsoft.Docking.dll", Path.Combine(Application.UserAppDataPath, "Revsoft.Docking.dll"));
				File.Delete("Revsoft.Docking.dll");
				File.Move(Path.Combine(Application.UserAppDataPath, "Revsoft.Docking.dll"), "Revsoft.Docking.dll");
            }
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
            try
			{
				Console.WriteLine("Downloading IWabbitemu.dll");
				Client.DownloadFile("http://group.revsoft.org/Wabbitcode/IWabbitemu.dll", Path.Combine(Application.UserAppDataPath, "IWabbitemu.dll"));
				File.Delete("IWabbitemu.dll");
				File.Move(Path.Combine(Application.UserAppDataPath, "IWabbitemu.dll"), "IWabbitemu.dll");
            }
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
            try
            {
				MessageBox.Show("Update Successful");
				Process newWabbit = new Process
										{
											StartInfo =
											{
												FileName = "Revsoft.Wabbitcode.exe",
												LoadUserProfile = true,
											}
										};
				newWabbit.Start();
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
		}
	}
}
