namespace Revsoft.Wabbitcode
{
    using Revsoft.Wabbitcode.Classes;
    using Revsoft.Wabbitcode.Properties;
    using Revsoft.Wabbitcode.Services;
    using System;
    using System.Threading.Tasks;
    using System.Windows.Forms;

    internal static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        private static void Main(string[] args)
        {
            try
            {
                if (Settings.Default.firstRun)
                {
                    Settings.Default.Upgrade();
                    Settings.Default.firstRun = false;
                    Settings.Default.Save();
                }
            }
            catch (Exception ex)
            {
                Services.DockingService.ShowError("Error upgrading settings", ex);
            }

            Application.EnableVisualStyles();
            FileLocations.InitDirs();
            FileLocations.InitFiles();
            HighlightingClass.MakeHighlightingFile();
            try
            {
                Task updateTask = Task.Factory.StartNew(() =>
                {
                    if (UpdateService.CheckForUpdate())
                    {
                        var result = MessageBox.Show("New version available. Download now?", "Update Available", MessageBoxButtons.YesNo, MessageBoxIcon.None);
                        if (result == System.Windows.Forms.DialogResult.Yes)
                        {
                            UpdateService.StartUpdater();
                            Application.Exit();
                            return;
                        }
                    }
                });
            }
            catch (Exception ex)
            {
                Services.DockingService.ShowError("Error checking for updates", ex);
            }

            try
            {
                AppBase appBase = new AppBase();
                appBase.Run(args);
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Unhandled exception occurred. Please report this to the developers", ex);
            }
        }
    }
}