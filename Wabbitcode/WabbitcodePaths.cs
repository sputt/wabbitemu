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
        const string templatesPath = "TemplatesConfig.xml";
        const string wabbitcodeDirName = "Wabbitcode";
        const string templateDirName = "Templates";
        const string includeDirName = "Includes";
        const string projectsName = "Projects";
        private static string ApplicationAppData
        {
            get
            {
                return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), Application.ResourceAssembly.GetName().Name);
            }
        }

        public static string TemplatesConfig
        {
            get
            {
                return Path.Combine(TemplatesDirectory, templatesPath);
            }
        }

        public static string DockConfig
        {
            get
            {
                return Path.Combine(ApplicationAppData, dockConfigPath);
            }
        }

        public static string WabbitcodeDirectory
        {
            get
            {
                return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), wabbitcodeDirName);
            }
        }

        public static string ProjectDirectory
        {
            get
            {
                return Path.Combine(WabbitcodeDirectory, projectsName);
            }
        }

        public static string TemplatesDirectory
        {
            get
            {
                return Path.Combine(WabbitcodeDirectory, templateDirName);
            }
        }

        public static string IncludeDirectory
        {
            get
            {
                return Path.Combine(WabbitcodeDirectory, includeDirName);
            }
        }

        public static void InitPaths()
        {
            Directory.CreateDirectory(ApplicationAppData);
            Directory.CreateDirectory(WabbitcodeDirectory);
            Directory.CreateDirectory(TemplatesDirectory);
            Directory.CreateDirectory(IncludeDirectory);
        }
    }
}
