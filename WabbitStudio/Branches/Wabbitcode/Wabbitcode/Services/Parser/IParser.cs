using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Parser
{
    public interface IParser
    {
        event ParserProgressHandler OnParserProgress;

        void ParseText(ParserInformation parserInfo, string fileText);

        List<Reference> FindReferences(FilePath fileName, string fileText, string textToFind, bool caseSensitive);
    }
}
