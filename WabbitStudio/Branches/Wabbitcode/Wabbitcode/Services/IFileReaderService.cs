using Revsoft.Wabbitcode.Services.Interface;

namespace Revsoft.Wabbitcode.Services
{
    public interface IFileReaderService : IService
    {
        string GetLine(string fileName, int lineNumber);
    }
}