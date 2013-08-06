using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Utils
{
    public class Errors
    {
        public Errors(string file, int lineNum, string description, bool isWarning)
        {
            File = file;
            LineNum = lineNum;
            Description = description;
            IsWarning = isWarning;
            ToolTip = new ToolTip
            {
                ToolTipTitle = description
            };
        }

        public string Description
        {
            get;
            set;
        }

        public string File
        {
            get;
            private set;
        }

        public bool IsWarning
        {
            get;
            private set;
        }

        public int LineNum
        {
            get;
            private set;
        }

        public ToolTip ToolTip
        {
            get;
            private set;
        }
    }
}