using System;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Reflection;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Services
{
    public static class UpdateService
    {
        private const string Hostname = "http://buckeyedude.zapto.org/Revsoft/Wabbitcode/";

        private static readonly object UpdatingLock = new object();

        public static bool CheckForUpdate()
        {
            lock (UpdatingLock)
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
                        Hostname),
                    FileName = "Revsoft.Autoupdater.exe"
                }
            };
            updater.Start();
            Application.Exit();
        }

        private static UpdateState CheckForNewerVersion()
        {
            UpdateState updateState = null;
            Version curVersion = Assembly.GetExecutingAssembly().GetName().Version;
            WebClient client = null;
            try
            {
                client = new WebClient();
                Version newVer = new Version(client.DownloadString(String.Format("{0}WabbitcodeVersion.txt", Hostname)));
                updateState = new UpdateState(curVersion.CompareTo(newVer) < 0);
            }
            catch (WebException)
            {
                return new UpdateState(false);
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Unable to check for update", ex);
            }
            finally
            {
                if (client != null)
                {
                    client.Dispose();
                }
            }
            return updateState;
        }

        private sealed class UpdateState
        {
            public bool IsNewerVersion { get; private set; }

            public UpdateState(bool isNewerVersion)
            {
                IsNewerVersion = isNewerVersion;
            }
        }
    }
}