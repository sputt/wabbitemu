using System;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Interfaces;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class ToolWindow : DockContent, IClipboardOperation
    {
        #region Events

        public new static event EventHandler OnDockStateChanged;

        #endregion

        protected readonly IDockingService DockingService;

        protected ToolWindow()
        {
            InitializeComponent();

            DockingService = DependencyFactory.Resolve<IDockingService>();
        }

        protected override string GetPersistString()
        {
            return GetType().FullName;
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

        protected virtual void EnablePanel(bool enabled)
        {
            if (enabled == Enabled)
            {
                return;
            }

            if (InvokeRequired)
            {
                this.Invoke(() => EnablePanel(enabled));
                return;
            }

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

        private void ToolWindow_DockStateChanged(object sender, EventArgs e)
        {
            if (OnDockStateChanged != null)
            {
                OnDockStateChanged(sender, e);
            }
        }
    }
}