using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services.Parser
{
    public interface IParser
    {
        event ParserProgressHandler OnParserProgress;

        void ParseText(ParserInformation parserInfo, string fileText);

        List<Reference> FindReferences(string fileName, string fileText, string textToFind, bool caseSensitive);
    }
}
