using System;
using System.IO;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Utils
{
	public static class FileLocations
	{
		private static readonly string WabbitcodeFolder = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "Wabbitcode");

		public static string ConfigFile
		{
			get
			{
				return Settings.Default.configLoc.Replace("%docs%", Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments));
			}
		}

		public static string IncludesDir
		{
			get
			{
				return Path.Combine(Settings.Default.createFolders ? 
					WabbitcodeFolder : 
					Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Includes");
			}
		}

		public static string ProjectsDir
		{
			get
			{
				return Settings.Default.createFolders ? 
					Path.Combine(WabbitcodeFolder, "Projects") : 
					Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Templates");
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
				return Path.Combine(Settings.Default.createFolders ?
					WabbitcodeFolder :
					Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Templates");
			}
		}

		private static string WabbitemuFile
		{
			get
			{
				return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "wabbitemu.exe");
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
					DockingService.ShowError("Unable to create directories", ex);
				}
			}
		}

		internal static void InitFiles()
		{
			try
			{
				if (!File.Exists(SpasmFile))
				{
					Resources.Resources.GetResource("spasm.exe", SpasmFile);
				}

				if (!File.Exists(WabbitemuFile))
				{
					Resources.Resources.GetResource("Wabbitemu.exe", WabbitemuFile);
				}
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Unable to initialize files", ex);
			}
		}
	}
}