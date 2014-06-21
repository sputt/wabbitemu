using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using Revsoft.Wabbitcode.Annotations;

namespace Revsoft.Wabbitcode.Utils
{
    [UsedImplicitly]
    public class FileTypeMethodFactory : IFileTypeMethodFactory
    {
        public delegate bool FileHandlerDelegate(FilePath fileName);

        private readonly Dictionary<string, FileHandlerDelegate> _fileMethods = new Dictionary<string, FileHandlerDelegate>();
        private FileHandlerDelegate _defaultHandler;

        public void RegisterFileType(string extension, FileHandlerDelegate handler)
        {
            if (_fileMethods.ContainsKey(extension))
            {
                throw new ArgumentException("File extension is already registered");
            }

            if (handler == null)
            {
                throw new ArgumentNullException("handler");
            }

            _fileMethods.Add(extension, handler);
        }

        public void RegisterDefaultHandler(FileHandlerDelegate handler)
        {
            if (handler == null)
            {
                throw new ArgumentNullException("handler");
            }

            _defaultHandler = handler;
        }

        public bool OpenRegisteredFile(FilePath fileName)
        {
            string extension = Path.GetExtension(fileName);
            if (extension == null)
            {
                return false;
            }

            if (!_fileMethods.ContainsKey(extension))
            {
                return _defaultHandler != null && _defaultHandler(fileName);
            }

            FileHandlerDelegate handler;
            _fileMethods.TryGetValue(extension, out handler);
            Debug.Assert(handler != null);
            return handler(fileName);
        }
    }
}