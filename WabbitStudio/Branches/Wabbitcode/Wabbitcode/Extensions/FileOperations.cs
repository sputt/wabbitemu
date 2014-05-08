using System;
using System.IO;
using System.Text;
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
		        uriString = uriString.Remove(0, dir.Length + 1);		// +1 for /
	        }

	        return uriString;

            /*if (absolutePath == relativeTo)
                return "";
            string[] absoluteDirectories = absolutePath.Split('\\');
            string[] relativeDirectories = relativeTo.Split('\\');
            //Get the shortest of the two paths
            int length = absoluteDirectories.Length < relativeDirectories.Length ? absoluteDirectories.Length : relativeDirectories.Length;
            //Use to determine where in the loop we exited
            int lastCommonRoot = -1;
            int index;
            //Find common root
            for (index = 0; index < length; index++)
                if (absoluteDirectories[index] == relativeDirectories[index])
                    lastCommonRoot = index;
                else
                    break;
            //If we didn't find a common prefix then throw
            if (lastCommonRoot == -1)
                throw new ArgumentException("Paths do not have a common base");
            //Build up the relative path
            StringBuilder relativePath = new StringBuilder();
            //Add on the ..
            for (index = lastCommonRoot + 1; index < absoluteDirectories.Length; index++)
                if (absoluteDirectories[index].Length > 0)
                    relativePath.Append("..\\");            //Add on the folders
            for (index = lastCommonRoot + 1; index < relativeDirectories.Length - 1; index++)
                relativePath.Append(relativeDirectories[index] + "\\");
            relativePath.Append(relativeDirectories[relativeDirectories.Length - 1]);
            return relativePath.ToString();*/
        }

    }
}