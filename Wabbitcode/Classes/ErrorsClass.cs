using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Classes
{
    public class Errors
    {
        public Errors(string file, int lineNum, string description, bool isWarning)
        {
            this.File = file;
            this.LineNum = lineNum;
            this.Description = description;
            this.IsWarning = isWarning;
            ToolTip = new ToolTip()
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