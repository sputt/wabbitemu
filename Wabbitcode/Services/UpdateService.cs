namespace Revsoft.Wabbitcode.Services
{
    using System;
    using System.Diagnostics;
    using System.IO;
    using System.Net;
    using System.Reflection;
    using System.Threading;
    using System.Windows.Forms;

    public static class UpdateService
    {
        private const string hostname = "http://buckeyedude.zapto.org/Revsoft/Wabbitcode/";

        private static bool updating;

        public static bool CheckForUpdate()
        {
            if (updating)
            {
                throw new Exception("Already updating");
            }

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
                StartInfo =
                {
                    WorkingDirectory = Directory.GetCurrentDirectory(),
                    Arguments = string.Format(
                        "-R Revsoft.Wabbitcode.exe Revsoft.Wabbitcode.exe {0}Revsoft.Wabbitcode.exe " +
                        "Revsoft.TextEditor.dll {0}Revsoft.TextEditor.dll WeifenLuo.WinFormsUI.Docking.dll {0}WeifenLuo.WinFormsUI.Docking.dll IWabbitemu.dll {0}IWabbitemu.dll",
                        hostname),
                    FileName = "Revsoft.Autoupdater.exe"
                }
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
                Version newVer = new Version(Client.DownloadString(String.Format("{0}WabbitcodeVersion.txt", hostname)));
                updateState.IsNewerVersion = curVersion.CompareTo(newVer) < 0;
                updateState.NewerVersion = newVer;
            }
            catch (WebException)
            {
                return;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            finally
            {
                if (Client != null)
                {
                    Client.Dispose();
                }

                if (sr != null)
                {
                    sr.Close();
                }

                updateState.eventWaitHandle.Set(); // signal we're done
            }
        }

        public sealed class UpdateState
        {
            public EventWaitHandle eventWaitHandle = new ManualResetEvent(false);
            public bool IsNewerVersion;
            public Version NewerVersion;
        }
    }
}