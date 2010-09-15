#define WABBITEMU
using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Windows.Forms;
using System.Net;
using System.Security.Principal;
using System.Security;
using System.Security.AccessControl;
using System.Security.Permissions;

namespace Revsoft.AutoUpdater
{
	class Program
	{
		static void Main(string[] args)
		{
            string filePath;
#if WABBITEMU
            filePath = "Wabbitemu.exe";
#endif
            bool isElevated;
            WindowsPrincipal principal = new WindowsPrincipal(WindowsIdentity.GetCurrent());
            isElevated = principal.IsInRole(WindowsBuiltInRole.Administrator);
            bool requiresAdmin = false;
            FileStream stream = null;
            try
            {
                string tempFile = Path.Combine(Application.StartupPath, "temp.txt");
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
            foreach (string arg in args)
                if (arg == "-A" && !isElevated)
                    requiresAdmin = true;
                else
                    filePath = arg;
            if (requiresAdmin)
            {
                Process process = new Process()
                {
                    StartInfo =
                    {
                        UseShellExecute = true,
                        Verb = "runas",
                        FileName = "Revsoft.AutoUpdater.exe",
                        Arguments = "-A"
                    }
                };
                process.Start();
                return;
            }
			if (args.Length > 0 && string.IsNullOrEmpty(args[0]))
			{
				Process[] wabbitcode = Process.GetProcesses();
				foreach (Process openWabbit in wabbitcode)
					if (openWabbit.ProcessName.ToLower().Contains("wabbitcode"))
						openWabbit.Close();
			}
			Thread.Sleep(5000);
            try
            {
                bool succeeded = DownloadAndUpdate(filePath);
                if (succeeded)
                    MessageBox.Show("Update Successful");
                else
                    MessageBox.Show("Update Failed");
				Process newWabbit = new Process
										{
											StartInfo =
											{
												FileName = "Revsoft.Wabbitcode.exe",
											}
										};
				newWabbit.Start();
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
		}

        static bool DownloadAndUpdate(string filePath)
        {
            bool succeeded = true;
#if WABBITCODE
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
                succeeded = false;
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
                succeeded = false;
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
                succeeded = false;
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
                succeeded = false;
            }
#elif WABBITEMU
            WebClient Client = new WebClient();
            try
            {
                Console.WriteLine("Downloading Wabbitemu.exe");
                Client.DownloadFile("http://wabbit.codeplex.com/releases/view/44625#DownloadId=122222", Path.Combine(Application.UserAppDataPath, "Revsoft.Wabbitcode.exe"));
                File.Delete(filePath);
                File.Move(Path.Combine(Application.UserAppDataPath, "Wabbitemu.exe"), filePath);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
                succeeded = false;
            }
#endif
            return succeeded;
        }
	}
}
