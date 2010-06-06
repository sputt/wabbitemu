using System;
using Revsoft.Docking;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class ToolWindow : DockContent
    {
        public ToolWindow()
        {
            InitializeComponent();
        }
		
		private void ToolWindow_VisibleChanged(object sender, EventArgs e)
		{
			DockingService.MainForm.UpdateChecks();
		}

        private void autoHideItem_Click(object sender, EventArgs e)
        {
            switch (DockState)
            {
                case DockState.DockBottom:
                    DockState = DockState.DockBottomAutoHide;
                    break;
                case DockState.DockBottomAutoHide:
                    DockState = DockState.DockBottom;
                    break;
                case DockState.DockLeft:
                    DockState = DockState.DockLeftAutoHide;
                    break;
                case DockState.DockLeftAutoHide:
                    DockState = DockState.DockLeft;
                    break;
                case DockState.DockRight:
                    DockState = DockState.DockRightAutoHide;
                    break;
                case DockState.DockRightAutoHide:
                    DockState = DockState.DockRight;
                    break;
                case DockState.DockTop:
                    DockState = DockState.DockTopAutoHide;
                    break;
                case DockState.DockTopAutoHide:
                    DockState = DockState.DockTop;
                    break;
            }
        }

		public virtual void Cut() { }
		public virtual void Copy() { }
		public virtual void Paste() { }

        private void floatingItem_Click(object sender, EventArgs e)
        {
            DockState = DockState.Float;
        }

        private void hideItem_Click(object sender, EventArgs e)
        {
            Hide();
        }
    }
}