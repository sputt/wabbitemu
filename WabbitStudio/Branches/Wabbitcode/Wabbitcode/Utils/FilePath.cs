using System;
using System.IO;
using System.Text;

namespace Revsoft.Wabbitcode.Utils
{
    public class FilePath
    {
        private readonly string _path;

        public FilePath(string path)
        {
            _path = path;
        }

        public override string ToString()
        {
            return _path;
        }

        public static implicit operator string(FilePath filePath)
        {
            return filePath == null ? null : filePath._path;
        }

        public static bool operator ==(FilePath path1, FilePath path2)
        {
            if ((object)path1 == null || (object)path2 == null)
            {
                return (object)path1 == null && (object)path2 == null;
            }

            return path1.Equals(path2);
        }

        public static bool operator !=(FilePath path1, FilePath path2)
        {
            if (path1 == null || path2 == null)
            {
                return !(path1 == null && path2 == null);
            }

            return !path1.Equals(path2);
        }

        public override bool Equals(object obj)
        {
            FilePath path = obj as FilePath;

            if (path == null)
            {
                return false;
            }

            return string.Compare(
                Path.GetFullPath(this).TrimEnd('\\'),
                Path.GetFullPath(path).TrimEnd('\\'),
                StringComparison.InvariantCultureIgnoreCase) == 0;
        }

        public override int GetHashCode()
        {
            return (_path != null ? _path.ToLower().GetHashCode() : 0);
        }

        public FilePath ChangeExtension(string extension)
        {
            return new FilePath(Path.ChangeExtension(_path, extension));
        }

        public FilePath GetDirectoryName()
        {
            return new FilePath(Path.GetDirectoryName(_path));
        }

        public FilePath Combine(FilePath path)
        {
            return new FilePath(Path.Combine(_path, path));
        }

        public FilePath Combine(string path)
        {
            return new FilePath(Path.Combine(_path, path));
        }

        /// <summary>
        /// Gets the normalized version of fileName.
        /// Slashes are replaced with backslashes, backreferences "." and ".." are 'evaluated'.
        /// </summary>
        public FilePath NormalizePath()
        {
            if (string.IsNullOrEmpty(_path))
            {
                return new FilePath(_path);
            }

            int i;
            bool isWeb = false;

            for (i = 0; i < _path.Length; i++)
            {
                if (_path[i] == '/' || _path[i] == '\\')
                {
                    break;
                }

                if (_path[i] == ':')
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
            if ((isWeb == false && _path.StartsWith(@"\\")) || _path.StartsWith("//"))
            {
                i = 2;
                result.Append(outputSeparator);
            }
            else
            {
                i = 0;
            }

            int segmentStartPos = i;

            for (; i <= _path.Length; i++)
            {
                if (i == _path.Length || _path[i] == '/' || _path[i] == '\\')
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
                            if (_path[segmentStartPos] != '.')
                            {
                                if (result.Length > 0)
                                {
                                    result.Append(outputSeparator);
                                }

                                result.Append(_path[segmentStartPos]);
                            }

                            break;
                        case 2:
                            if (_path[segmentStartPos] == '.' && _path[segmentStartPos + 1] == '.')
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

                            result.Append(_path, segmentStartPos, segmentLength);
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

            return new FilePath(result.ToString());
        }

        public FilePath GetAbsolutePath(FilePath relPath)
        {
            return Combine(relPath).NormalizePath();
        }
    }
}
