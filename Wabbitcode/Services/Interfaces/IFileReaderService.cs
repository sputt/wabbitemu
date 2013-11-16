namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public interface IFileReaderService : IService
    {
        /// <summary>
        /// Returns the string contents of a line from a given file
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="lineNumber"></param>
        /// <returns></returns>
        string GetLine(string fileName, int lineNumber);

        string GetFileText(string fileName);
    }
}