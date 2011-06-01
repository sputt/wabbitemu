using System;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
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
			AppBase appBase = new AppBase();
            appBase.Run(args);
        }
    }
}