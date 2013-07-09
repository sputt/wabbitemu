namespace Revsoft.Wabbitcode.Classes
{
    using Revsoft.Wabbitcode.Properties;
    using System;
    using System.IO;

    public static class FileLocations
    {
        private static string wabbitcodeFolder = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "Wabbitcode");

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
                if (Settings.Default.createFolders)
                {
                    return Path.Combine(wabbitcodeFolder, "Includes");
                }
                else
                {
                    return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Includes");
                }
            }
        }

        public static string ProjectsDir
        {
            get
            {
                if (Settings.Default.createFolders)
                {
                    return Path.Combine(wabbitcodeFolder, "Projects");
                }
                else
                {
                    return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Templates");
                }
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
                {
                    return Path.Combine(wabbitcodeFolder, "Templates");
                }
                else
                {
                    return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Templates");
                }
            }
        }

        public static string WabbitemuFile
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
                    Directory.CreateDirectory(wabbitcodeFolder);
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
                {
                    Classes.Resources.GetResource("spasm.exe", FileLocations.SpasmFile);
                }

                if (!File.Exists(FileLocations.WabbitemuFile))
                {
                    Classes.Resources.GetResource("Wabbitemu.exe", FileLocations.WabbitemuFile);
                }
            }
            catch (Exception ex)
            {
                Services.DockingService.ShowError("Unable to initialize files", ex);
            }
        }
    }
}