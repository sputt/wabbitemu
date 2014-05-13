namespace Revsoft.Wabbitcode.Utils
{
    public interface IFileTypeMethodFactory
    {
        void RegisterFileType(string extension, FileTypeMethodFactory.FileHandlerDelegate handler);
        void RegisterDefaultHandler(FileTypeMethodFactory.FileHandlerDelegate handler);
        bool OpenRegisteredFile(FilePath fileName);
    }
}