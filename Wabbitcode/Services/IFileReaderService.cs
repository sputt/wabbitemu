using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
    public interface IFileReaderService : IService
    {
        string GetLine(string fileName, int lineNumber);
    }
}