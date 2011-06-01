using System;
using System.IO;
using Revsoft.Wabbitcode.Properties;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Classes
{
    public static class FileLocations
    {
        public static string WabbitcodeFolder = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "Wabbitcode");
        public static string WabbitemuFile
		{
			get
			{
				return Settings.Default.wabbitLoc.Replace("%docs%", Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments));
			}
		}
		public static string SpasmFile
		{
			get
			{
				return Settings.Default.assemblerLoc.Replace("%docs%", Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments));
			}
		}
        public static string TemplatesDir
		{
			get
			{
				if (Settings.Default.createFolders)
					return Path.Combine(WabbitcodeFolder, "Templates");
				else
					return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Templates");
			}
		}
        public static string IncludesDir
		{
			get
			{
				if (Settings.Default.createFolders)
					return Path.Combine(WabbitcodeFolder, "Includes");
				else
					return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Includes");
			}
		}
        public static string ProjectsDir 
		{
			get
			{
				if (Settings.Default.createFolders)
					return Path.Combine(WabbitcodeFolder, "Projects");
				else
					return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Templates");
			}
		}
		public static string ConfigFile
		{
			get
			{
				return Settings.Default.configLoc.Replace("%docs%", Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments));
			}
		}

        internal static void InitDirs()
        {
			if (Settings.Default.createFolders)
			{
				try
				{
					Directory.CreateDirectory(WabbitcodeFolder);
					Directory.CreateDirectory(TemplatesDir);
					Directory.CreateDirectory(IncludesDir);
					Directory.CreateDirectory(ProjectsDir);
				}
				catch (Exception ex)
				{
					Services.DockingService.ShowError("Unable to create directories", ex);
				}
			}
        }

		internal static void InitFiles()
		{
			string dir = Path.GetDirectoryName(SpasmFile);
			if (!Directory.Exists(dir))
				if (MessageBox.Show("Directory '" + dir + "' does not exist. Would you like to create it?", "Directory does not exist", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation) != DialogResult.Yes)
					Settings.Default.assemblerLoc = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "spasm.exe");
			Classes.Resources.GetResource("spasm.exe", FileLocations.SpasmFile);
			dir = Path.GetDirectoryName(WabbitemuFile);
			if (!Directory.Exists(dir))
				if (MessageBox.Show("Directory '" + dir + "' does not exist. Would you like to create it?", "Directory does not exist", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation) != DialogResult.Yes)
					Settings.Default.wabbitLoc = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "wabbitemu.exe");
			Classes.Resources.GetResource("Wabbitemu.exe", FileLocations.WabbitemuFile);
		}
    }
}
