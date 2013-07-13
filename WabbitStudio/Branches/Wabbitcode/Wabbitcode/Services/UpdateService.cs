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

        private static object updatingLock = new object();

        public static bool CheckForUpdate()
        {
            lock (updatingLock)
            {
                UpdateState state = CheckForNewerVersion();
                return state.IsNewerVersion;
            }
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

        private static UpdateState CheckForNewerVersion()
        {
            UpdateState updateState = null;
            Version curVersion =  Assembly.GetExecutingAssembly().GetName().Version;
            WebClient Client = null;
            StreamReader sr = null;
            try
            {
                Client = new WebClient();
                Version newVer = new Version(Client.DownloadString(String.Format("{0}WabbitcodeVersion.txt", hostname)));
                updateState = new UpdateState(curVersion.CompareTo(newVer) < 0, newVer);
            }
            catch (WebException)
            {
                return new UpdateState(false, null);
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
            }
            return updateState;
        }

        public sealed class UpdateState
        {
            public bool IsNewerVersion { get; private set; }
            public Version NewestVersion { get; private set; }

            public UpdateState(bool isNewerVersion, Version newestVersion)
            {
                IsNewerVersion = isNewerVersion;
                NewestVersion = newestVersion;
            }
        }
    }
}