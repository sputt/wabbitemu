using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public interface IFileService
    {
        /// <summary>
        /// Returns the string contents of a line from a given file
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="lineNumber"></param>
        /// <returns></returns>
        string GetLine(FilePath fileName, int lineNumber);

        string GetFileText(FilePath fileName);

        IDocument GetOpenDocument(FilePath fileName);
        void SetFileText(FilePath fileName, string text);
    }
}