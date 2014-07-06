namespace Revsoft.Wabbitcode.Utils
{
    public delegate bool FileHandlerDelegate(FilePath fileName);

    public interface IFileTypeMethodFactory
    {
        void RegisterFileType(string extension, FileHandlerDelegate handler);
        void RegisterDefaultHandler(FileHandlerDelegate handler);
        bool OpenRegisteredFile(FilePath fileName);
    }
}