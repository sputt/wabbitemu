using System;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.DockingWindows
{
	public partial class ToolWindow : DockContent, IClipboardOperation
    {
        #region Events

	    public static event EventHandler OnVisibilityChanged;

        #endregion

        protected readonly IDockingService DockingService;

		protected ToolWindow()
		{
			InitializeComponent();

			DockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
		}

		public virtual void Copy()
		{
			// no-op
		}

		public virtual void Cut()
		{
			// no-op
		}

		public virtual void Paste()
		{
			// no-op
		}

	    public void EnablePanel(bool enabled)
	    {
	        Enabled = enabled;
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

		private void floatingItem_Click(object sender, EventArgs e)
		{
			DockState = DockState.Float;
		}

		private void hideItem_Click(object sender, EventArgs e)
		{
			Hide();
		}

		private void ToolWindow_VisibleChanged(object sender, EventArgs e)
		{
		    if (OnVisibilityChanged != null)
		    {
		        OnVisibilityChanged(sender, e);
		    }
		}
	}
}