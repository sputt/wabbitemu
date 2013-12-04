using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
	public interface IDocumentService : IService
	{
		Editor CreateNewDocument();
		Editor GotoFile(string file);
		void GotoLabel(IParserData item);
		void GotoLine(string file, int scrollToLine);
		void HighlightDebugLine(int newLineNumber);
		void RemoveDebugHighlight();

	    Editor OpenDocument(string filename);
	}
}