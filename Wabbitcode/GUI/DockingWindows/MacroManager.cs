using System;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
	public partial class MacroManager : ToolWindow
	{
	    public const string WindowIdentifier = "Macro Manager";
        public override string WindowName
        {
            get
            {
                return WindowIdentifier;
            }
        }

		public MacroManager()
		{
			InitializeComponent();
		}

		private void deleteMacroButton_Click(object sender, EventArgs e)
		{
		}

		private void newMacroButton_Click(object sender, EventArgs e)
		{
		}

		private void runMacroButton_Click(object sender, EventArgs e)
		{
		}
	}
}