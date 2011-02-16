using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services
{
    public static class PropertyService
    {
        public static dynamic GetWabbitcodeProperty(string pName)
        {
            return Properties.Settings.Default[pName];
        }

        public static dynamic GetEditorProperty(string pName)
        {
            return Properties.Editor.Default[pName];
        }

        public static void SaveWabbitcodeProperty(string pName, object data)
        {
            Properties.Settings.Default[pName] = data;
        }

        public static void SaveEditorProperty(string pName, object data)
        {
            Properties.Editor.Default[pName] = data;
        }

        public static void Save()
        {
            Properties.Settings.Default.Save();
            Properties.Editor.Default.Save();
        }
    }
}
