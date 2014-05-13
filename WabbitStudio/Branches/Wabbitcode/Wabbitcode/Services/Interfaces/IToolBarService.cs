using System;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public interface IToolBarService
    {
        event EventHandler<ToolbarEventArgs> OnToolbarRegistered;
        event EventHandler<ToolbarVisibilityChangedEventArgs> OnToolBarVisibilityChanged;

        void RegisterToolbar(string toolBarName, ToolStrip toolBar);

        void ShowToolBar(string toolBarName);
        void HideToolBar(string toolBarName);
    }

    public class ToolbarEventArgs : EventArgs
    {
        public string ToolBarName { get; private set; }

        public ToolbarEventArgs(string toolbarName)
        {
            ToolBarName = toolbarName;
        }
    }

    public class ToolbarVisibilityChangedEventArgs : ToolbarEventArgs
    {
        public bool Visible { get; private set; }

        public ToolbarVisibilityChangedEventArgs(string toolbarName, bool visible)
            : base(toolbarName)
        {
            Visible = visible;
        }
    }
}