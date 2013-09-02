using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services.Interface
{
	public interface IDocumentService : IService
	{
		Editor ActiveDocument { get; }
		string ActiveFileName { get; }
		Editor CreateNewDocument();
		Editor GotoFile(string file);
		void GotoLabel(IParserData item);
		void GotoLine(Editor editor, int line);
		void GotoLine(string file, int scrollToLine);
		void HighlightDebugLine(int newLineNumber);
		void RemoveDebugHighlight();

		/// <summary>
		/// This loads the recent file list from Properties and adds it to the recent file menu.
		/// </summary>
		void GetRecentFiles();

		void GotoCurrentDebugLine();
		void HighlightCall();
		Editor OpenDocument(string filename);
		void OpenDocument(Editor doc, string filename);
	}
}