using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Docking_Windows;
using System.Text;
using System.Runtime.InteropServices;

namespace Revsoft.Wabbitcode.Classes
{	
	public class Errors
    {
        public string file;
        public int lineNum;
        public bool isWarning;
        public string description;
        public ToolTip toolTip;
        public Errors(string file, int lineNum, string description, bool isWarning)
        {
            this.file = file;
            this.lineNum = lineNum;
            this.description = description;
            this.isWarning = isWarning;
            toolTip = new ToolTip {ToolTipTitle = description};
        }
    }
}
