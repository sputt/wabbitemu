using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Classes
{
    public static class FileLocations
    {
        public static string WabbitcodeFolder = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), "Wabbitcode");
        public static string WabbitemuFile = Path.Combine(WabbitcodeFolder, "Wabbitemu.exe");
        public static string SpasmFile = Path.Combine(WabbitcodeFolder, "spasm.exe");
        public static string TemplatesDir = Path.Combine(WabbitcodeFolder, "Templates");
        public static string IncludesDir = Path.Combine(WabbitcodeFolder, "Includes");
        public static string ProjectsDir = Path.Combine(WabbitcodeFolder, "Projects");
        public static string ConfigFile = Path.Combine(WabbitcodeFolder, "DockPanel.config");

        internal static void InitDirs()
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
}
