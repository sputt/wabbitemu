using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Threading;
using System.Threading;
using System.Xml;

namespace Revsoft.Wabbitcode
{
    public static class ExtensionMethods
    {
        public static bool InvokeRequired(this Dispatcher dispatcher)
        {
            return dispatcher.Thread != Thread.CurrentThread;
        }

        public static bool MoveToNextElement(this XmlTextReader reader)
        {
            if (!reader.Read())
                return false;

            while (reader.NodeType == XmlNodeType.EndElement)
            {
                if (!reader.Read())
                    return false;
            }
            return true;
        }

        public static string CleanFilename(this string filename)
        {
            string file = filename;
            file = string.Concat(file.Split(System.IO.Path.GetInvalidFileNameChars(), StringSplitOptions.RemoveEmptyEntries));
            if (file.Length > 250)
                file = file.Substring(0, 250);
            return file;
        }

        public static string CleanPath(this string path)
        {
            string file = path;
            file = string.Concat(file.Split(System.IO.Path.GetInvalidPathChars(), StringSplitOptions.RemoveEmptyEntries));
            if (file.Length > 250)
                file = file.Substring(0, 250);
            return file;
        }
    }
}
