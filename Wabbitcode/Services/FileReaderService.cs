using System;
using System.Collections.Generic;
using System.IO;

namespace Revsoft.Wabbitcode.Services
{
    public class FileReaderService : IFileReaderService
    {
        private readonly Dictionary<string, Tuple<string[], DateTime>> _cachedFiles = new Dictionary<string, Tuple<string[], DateTime>>();

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
                lines = _cachedFiles[fileName.ToLower()].Item1;
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

            var time = File.GetLastWriteTime(fileName);
            _cachedFiles.Add(fileName.ToLower(), new Tuple<string[], DateTime>(lines, time));
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
