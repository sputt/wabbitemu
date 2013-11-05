using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services.Interfaces
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
        /// Sanity checks on the list of recent files
        /// </summary>
        /// <returns>A list of string representing the last open files</returns>
		IEnumerable<string> GetRecentFiles();

		void GotoCurrentDebugLine();
		void HighlightCall();
		Editor OpenDocument(string filename);
		void OpenDocument(Editor doc, string filename);
	}
}