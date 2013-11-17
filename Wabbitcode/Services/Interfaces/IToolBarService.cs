using System;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public interface IToolBarService : IService
    {
        event EventHandler<ToolbarVisibilityChangedEventArgs> OnToolBarVisibilityChanged;

        void RegisterToolbar(string toolBarName, ToolStrip toolBar);

        void ShowToolBar(string toolBarName);
        void HideToolBar(string toolBarName);
    }

    public class ToolbarVisibilityChangedEventArgs : EventArgs
    {
        public string ToolBarName { get; private set; }

        public bool Visible { get; private set; }

        public ToolbarVisibilityChangedEventArgs(string toolbarName, bool visible)
        {
            ToolBarName = toolbarName;
            Visible = visible;
        }
    }
}