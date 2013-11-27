using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

namespace Revsoft.Wabbitcode.Utils
{
    public static class FileTypeMethodFactory
    {
        public delegate bool FileHandlerDelegate(string fileName);

        private static readonly Dictionary<string, FileHandlerDelegate> FileMethods = new Dictionary<string, FileHandlerDelegate>();
        private static FileHandlerDelegate _defaultHandler;

        public static void RegisterFileType(string extension, FileHandlerDelegate handler)
        {
            if (FileMethods.ContainsKey(extension))
            {
                throw new ArgumentException("File extension is already registered");
            }

            if (handler == null)
            {
                throw new ArgumentNullException("handler");
            }

            FileMethods.Add(extension, handler);
        }

        public static void RegisterDefaultHandler(FileHandlerDelegate handler)
        {
            if (handler == null)
            {
                throw new ArgumentNullException("handler");
            }

            _defaultHandler = handler;
        }

        public static bool OpenRegisteredFile(string fileName)
        {
            string extension = Path.GetExtension(fileName);
            if (extension == null)
            {
                return false;
            }

            if (!FileMethods.ContainsKey(extension))
            {
                return _defaultHandler != null && _defaultHandler(fileName);
            }

            FileHandlerDelegate handler;
            FileMethods.TryGetValue(extension, out handler);
            Debug.Assert(handler != null);
            return handler(fileName);
        }
    }
}