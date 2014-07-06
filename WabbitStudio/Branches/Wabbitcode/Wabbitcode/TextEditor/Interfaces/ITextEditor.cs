using System.Windows.Forms;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Interfaces
{
    public interface ITextEditor
    {
        Control Parent { get; }

        int CaretLine { get; set; }
        int CaretColumn { get; set; }
        int CaretOffset { get; set; }
        int TotalLines { get; }
        FilePath FileName { get; }
        IDocument Document { get; }

        string GetWordAtCaret();

        void ShowFindForm(Form owner, SearchMode mode);

        string GetSelection();
        void SetSelection(int offset, int length);
        void SelectedTextToUpper();
        void SelectedTextToLower();
        void SelectedTextInvertCase();
        void SelectedTextToSentenceCase();

        void ConvertSpacesToTabs();
        void FormatLines();
        void GotoLine(int line);
    }
}