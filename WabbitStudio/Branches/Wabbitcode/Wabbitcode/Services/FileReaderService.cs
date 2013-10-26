using System;
using System.Collections.Generic;
using System.IO;

namespace Revsoft.Wabbitcode.Services
{
    public class FileReaderService : IFileReaderService
    {
        private readonly Dictionary<string, Tuple<string[], DateTime>> _cachedFiles = new Dictionary<string, Tuple<string[], DateTime>>();

        /// <summary>
        /// Returns the string contents of a line from a given file
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="lineNumber"></param>
        /// <returns></returns>
        public string GetLine(string fileName, int lineNumber)
        {
            // expected 1-index, convert to 0-index
            lineNumber--;
            if (lineNumber < 0)
            {
                return null;
            }

            fileName = fileName.ToLower();
            string[] lines;
            if (_cachedFiles.ContainsKey(fileName))
            {
                lines = _cachedFiles[fileName].Item1;
                DateTime currentWriteTime = _cachedFiles[fileName].Item2;
                DateTime lastWriteTime = File.GetLastWriteTime(fileName);
                if (currentWriteTime >= lastWriteTime)
                {
                    return lines[lineNumber];
                }

                _cachedFiles.Remove(fileName);
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

            DateTime time = File.GetLastWriteTime(fileName);
            _cachedFiles.Add(fileName, new Tuple<string[], DateTime>(lines, time));
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
