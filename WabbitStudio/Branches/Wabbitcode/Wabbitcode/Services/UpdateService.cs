using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Reflection;
using System.Diagnostics;
using System.Net;
using System.IO;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Services
{
	public static class UpdateService
	{
		public class UpdateState
		{
			public bool IsNewerVersion;
			public Version NewerVersion;
			public EventWaitHandle eventWaitHandle = new ManualResetEvent(false);
		}
		static bool updating;
		public static bool CheckForUpdate()
		{
			if (updating)
				throw new Exception("Already updating");
			updating = true;
			var state = new UpdateState();
			ThreadPool.QueueUserWorkItem(CheckForNewerVersion, state);
			state.eventWaitHandle.WaitOne();
			updating = false;
			return state.IsNewerVersion;
		}

        public static void StartUpdater()
        {
            Process updater = new Process
                                  {
                                      StartInfo = {
                                          WorkingDirectory = Directory.GetCurrentDirectory(),
                                          Arguments = "-R Revsoft.Wabbitcode.exe Revsoft.Wabbitcode.exe http://group.revsoft.org/Wabbitcode/Revsoft.Wabbitcode.exe Revsoft.TextEditor.dll http://group.revsoft.org/Wabbitcode/Revsoft.TextEditor.dll Revsoft.Docking.dll http://group.revsoft.org/Wabbitcode/Revsoft.Docking.dll IWabbitemu.dll http://group.revsoft.org/Wabbitcode/IWabbitemu.dll",
                                          FileName = "Revsoft.Autoupdater.exe" }
                                  };
            updater.Start();
			Application.Exit();
        }

        private static void CheckForNewerVersion(Object state)
		{
			UpdateState updateState = state as UpdateState;
			Version curVersion =  Assembly.GetExecutingAssembly().GetName().Version;
			WebClient Client = null;
			StreamReader sr = null;
			try
			{
				Client = new WebClient();
				Stream strm = Client.OpenRead("http://group.revsoft.org/Wabbitcode/WabbitcodeVersion.txt");
				sr = new StreamReader(strm);
				Version newVer = new Version(sr.ReadLine());
				updateState.IsNewerVersion = curVersion.CompareTo(newVer) < 0;
				updateState.NewerVersion = newVer;
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
			finally
			{
				if (Client != null)
					Client.Dispose();
				if (sr != null)
					sr.Close();
			}
			updateState.eventWaitHandle.Set(); // signal we're done
        }
	}
}
