using System;
using System.IO;
using Microsoft.Win32;

namespace Revsoft.Wabbitcode.Extensions
{
    public static class FileOperations
    {
        public static string GetMimeType(string fileName)
        {
            try
            {
                string mimeType = "application/unknown";
                var extension = Path.GetExtension(fileName);
                if (extension == null)
                {
                    return mimeType;
                }

                string ext = extension.ToLower();
                RegistryKey regKey = Registry.ClassesRoot.OpenSubKey(ext);
                if (regKey != null && regKey.GetValue("Content Type") != null)
                {
                    mimeType = regKey.GetValue("Content Type").ToString();
                }

                return mimeType;
            }
            catch (Exception)
            {
                return "application/unknown";
            }
        }

        public static string GetRelativePath(string absolutePath, string relativeTo)
        {
            Uri uri1 = new Uri(absolutePath);
            Uri uri2 = new Uri(relativeTo);
            Uri relativeUri = uri1.MakeRelativeUri(uri2);
            var uriString = Uri.UnescapeDataString(Uri.UnescapeDataString(relativeUri.OriginalString));
            var dir = Path.GetFileName(absolutePath);
            if (dir != null && !uriString.StartsWith(dir))
            {
                return uriString;
            }

            if (dir != null)
            {
                uriString = uriString.Remove(0, dir.Length + 1); // +1 for /
            }

            return uriString;
        }
    }
}