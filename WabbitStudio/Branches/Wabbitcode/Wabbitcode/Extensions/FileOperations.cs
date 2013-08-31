using System;
using System.IO;
using System.Text;
using Microsoft.Win32;

namespace Revsoft.Wabbitcode.Extensions
{
    public static class FileOperations
    {
        /// <summary>
        /// Combines baseDirectoryPath with relPath and normalizes the resulting path.
        /// </summary>
        public static string GetAbsolutePath(string baseDirectoryPath, string relPath)
        {
            return NormalizePath(Path.Combine(baseDirectoryPath, relPath));
        }

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

        /// <summary>
        /// Gets the normalized version of fileName.
        /// Slashes are replaced with backslashes, backreferences "." and ".." are 'evaluated'.
        /// </summary>
        public static string NormalizePath(string fileName)
        {
            if (string.IsNullOrEmpty(fileName))
            {
                return fileName;
            }

            int i;
            bool isWeb = false;

            for (i = 0; i < fileName.Length; i++)
            {
                if (fileName[i] == '/' || fileName[i] == '\\')
                {
                    break;
                }

                if (fileName[i] == ':')
                {
                    if (i > 1)
                    {
                        isWeb = true;
                    }

                    break;
                }
            }

            char outputSeparator = isWeb ? '/' : Path.DirectorySeparatorChar;

            StringBuilder result = new StringBuilder();
            if ((isWeb == false && fileName.StartsWith(@"\\")) || fileName.StartsWith("//"))
            {
                i = 2;
                result.Append(outputSeparator);
            }
            else
            {
                i = 0;
            }

            int segmentStartPos = i;

            for (; i <= fileName.Length; i++)
            {
                if (i == fileName.Length || fileName[i] == '/' || fileName[i] == '\\')
                {
                    int segmentLength = i - segmentStartPos;
                    switch (segmentLength)
                    {
                    case 0:
                        // ignore empty segment (if not in web mode)
                        // On unix, don't ignore empty segment if i==0
                        if (isWeb || (i == 0 && Environment.OSVersion.Platform == PlatformID.Unix))
                        {
                            result.Append(outputSeparator);
                        }

                        break;
                    case 1:
                        // ignore /./ segment, but append other one-letter segments
                        if (fileName[segmentStartPos] != '.')
                        {
                            if (result.Length > 0)
                            {
                                result.Append(outputSeparator);
                            }

                            result.Append(fileName[segmentStartPos]);
                        }

                        break;
                    case 2:
                        if (fileName[segmentStartPos] == '.' && fileName[segmentStartPos + 1] == '.')
                        {
                            // remove previous segment
                            int j;
                            for (j = result.Length - 1; j >= 0 && result[j] != outputSeparator; j--)
                            {
                            }

                            if (j > 0)
                            {
                                result.Length = j;
                            }

                            break;
                        }
		                // append normal segment
		                goto default;

	                    default:
                        if (result.Length > 0)
                        {
                            result.Append(outputSeparator);
                        }

                        result.Append(fileName, segmentStartPos, segmentLength);
                        break;
                    }

                    segmentStartPos = i + 1; // remember start position for next segment
                }
            }

            if (isWeb == false)
            {
                if (result.Length > 0 && result[result.Length - 1] == outputSeparator)
                {
                    result.Length -= 1;
                }

                if (result.Length == 2 && result[1] == ':')
                {
                    result.Append(outputSeparator);
                }
            }

            return result.ToString();
        }
    }
}