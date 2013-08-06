using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services.Interface
{
	public interface IDocumentService : IService
	{
		NewEditor ActiveDocument { get; }
		string ActiveFileName { get; }
		NewEditor CreateNewDocument();
		NewEditor GotoFile(string file);
		void GotoLabel(IParserData item);
		void GotoLine(int line);
		void GotoLine(string file, int scrollToLine);
		void HighlightDebugLine(int newLineNumber);
		void RemoveDebugHighlight();

		/// <summary>
		/// Save active document
		/// </summary>
		void SaveDocument();

		/// <summary>
		/// Save document as dialog. Also called if doc has never been saved.
		/// </summary>
		/// <returns></returns>
		void SaveDocumentAs();

		/// <summary>
		/// This loads the recent file list from Properties and adds it to the recent file menu.
		/// </summary>
		void GetRecentFiles();

		void GotoCurrentDebugLine();
		void HighlightCall();
		NewEditor OpenDocument(string filename);
		void OpenDocument(NewEditor doc, string filename);
		void SaveDocument(NewEditor doc);
	}
}