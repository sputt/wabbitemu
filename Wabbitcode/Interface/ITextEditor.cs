using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Interface
{
    public interface ITextEditor
    {
        int CaretLine { get; set;  }
        int CaretColumn { get; set; }
        int CaretOffset { get; set; }
        int TotalLines { get; }
        string FileName { get; }

        string GetWordAtCaret();

        void ShowFindForm(Form owner, SearchMode mode);

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