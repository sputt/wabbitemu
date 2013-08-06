using System;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interface;

namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class MacroManager : ToolWindow
	{
		public MacroManager(IDockingService dockingService)
			: base(dockingService)
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