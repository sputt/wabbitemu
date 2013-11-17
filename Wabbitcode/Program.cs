using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode
{
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
				if (Settings.Default.FirstRun)
				{
					Settings.Default.Upgrade();
					Settings.Default.FirstRun = false;
					Settings.Default.Save();
				}
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error upgrading settings", ex);
			}

			Application.EnableVisualStyles();
			FileLocations.InitDirs();
			FileLocations.InitFiles();
			HighlightingUtils.MakeHighlightingFile();
			try
			{
				Task.Factory.StartNew(() =>
				{
					if (!UpdateService.CheckForUpdate())
					{
						return;
					}

					var result = MessageBox.Show("New version available. Download now?", "Update Available", MessageBoxButtons.YesNo, MessageBoxIcon.None);
					if (result != DialogResult.Yes)
					{
						return;
					}

					UpdateService.StartUpdater();
					Application.Exit();
				});
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error checking for updates", ex);
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