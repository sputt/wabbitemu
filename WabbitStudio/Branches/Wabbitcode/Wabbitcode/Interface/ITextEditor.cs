using System.Windows.Forms;
using Revsoft.Wabbitcode.GUI.Dialogs;

namespace Revsoft.Wabbitcode.Interface
{
    public interface ITextEditor
    {
        Control Parent { get; }

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