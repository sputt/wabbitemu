using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows;
using System.Windows.Data;
using System.Globalization;

namespace Revsoft.Wabbitcode
{
    public static class WabbitcodePaths
    {
        const string dockConfigPath = "DockConfig.xml";
        private static string ApplicationAppData
        {
            get
            {
                return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), Application.ResourceAssembly.GetName().Name);
            }
        }

        public static string DockConfig
        {
            get
            {
                return Path.Combine(ApplicationAppData, dockConfigPath);
            }
        }

        public static void InitPaths()
        {
            Directory.CreateDirectory(ApplicationAppData);
        }
    }
}
