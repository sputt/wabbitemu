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
				return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "wabbitemu.exe");
				//return Settings.Default.wabbitLoc.Replace("%docs%", Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments));
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
			try
			{
				string dir = Path.GetDirectoryName(SpasmFile);
				if (!File.Exists(FileLocations.SpasmFile))
					Classes.Resources.GetResource("spasm.exe", FileLocations.SpasmFile);
				if (!File.Exists(FileLocations.WabbitemuFile))
					//Settings.Default.wabbitLoc = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "wabbitemu.exe");
					Classes.Resources.GetResource("Wabbitemu.exe", FileLocations.WabbitemuFile);
			}
			catch (Exception ex)
			{
				Services.DockingService.ShowError("Unable to initialize files", ex);
			}
		}
    }
}
