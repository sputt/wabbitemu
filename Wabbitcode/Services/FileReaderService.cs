using System;
using System.Collections.Generic;
using System.IO;

namespace Revsoft.Wabbitcode.Services
{
    public class FileReaderService : IFileReaderService
    {
        private readonly Dictionary<string, string[]> _cachedFiles = new Dictionary<string, string[]>();

        public string GetLine(string fileName, int lineNumber)
        {
            // expected 1-index, convert to 0-index
            lineNumber--;
            if (lineNumber < 0)
            {
                return null;
            }

            string[] lines;
            if (_cachedFiles.ContainsKey(fileName.ToLower()))
            {
                lines = _cachedFiles[fileName];
                return lines[lineNumber];
            }
            
            StreamReader reader = null;
            try
            {
                reader = new StreamReader(fileName);
                lines = reader.ReadToEnd().Split('\n');
            }
            catch (Exception)
            {
                return null;
            }
            finally
            {
                if (reader != null)
                {
                    reader.Dispose();
                }
            }

            _cachedFiles.Add(fileName.ToLower(), lines);
            return lines[lineNumber];
        }

        public void DestroyService()
        {   
        }

        public void InitService(params object[] objects)
        {
        }
    }
}
