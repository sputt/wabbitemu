using System;
using System.IO;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Utils
{
	public static class FileLocations
	{
		private static readonly string WabbitcodeFolder = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "Wabbitcode");
	    private static readonly string AppDataFolder = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
	    private static readonly string DocumentsFolder = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);

		public static string ConfigFile
		{
			get
			{
                return Settings.Default.ConfigLoc.Replace("%docs%", DocumentsFolder);
			}
		}

		public static FilePath IncludesDir
		{
			get
			{
			    string path = Path.Combine(Settings.Default.CreateFolders ?
                    WabbitcodeFolder : AppDataFolder, "Includes");
				return new FilePath(path);
			}
		}

		public static string ProjectsDir
		{
			get
			{
			    return Path.Combine(Settings.Default.CreateFolders ?
                    WabbitcodeFolder : AppDataFolder, "Projects");
			}
		}

		public static string SpasmFile
		{
			get
			{
				return Settings.Default.AssemblerLoc.Replace("%docs%", DocumentsFolder);
			}
		}

		public static string TemplatesDir
		{
			get
			{
				return Path.Combine(Settings.Default.CreateFolders ? 
                    WabbitcodeFolder : AppDataFolder, "Templates");
			}
		}

		private static string WabbitemuFile
		{
			get
			{
				return Path.Combine(AppDataFolder, "wabbitemu.exe");
			}
		}

		internal static void InitDirs()
		{
		    if (!Settings.Default.CreateFolders)
		    {
		        return;
		    }

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