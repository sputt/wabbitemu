using System;
using System.Collections.Generic;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public class ToolBarService : IToolBarService
    {
        private readonly Dictionary<string, ToolStrip> _toolBars = new Dictionary<string, ToolStrip>();
        private readonly ToolStripPanel _panel;

        public event EventHandler<ToolbarEventArgs> OnToolbarRegistered;
        public event EventHandler<ToolbarVisibilityChangedEventArgs> OnToolBarVisibilityChanged;

        public ToolBarService(ToolStripPanel panel)
        {
            _panel = panel;
        }

        public void RegisterToolbar(string toolBarName, ToolStrip toolBar)
        {
            if (_toolBars.ContainsKey(toolBarName))
            {
                throw new ArgumentException("Toolbar is already registered");
            }

            if (OnToolbarRegistered != null)
            {
                OnToolbarRegistered(this, new ToolbarEventArgs(toolBarName));
            }

            _toolBars.Add(toolBarName, toolBar);
            _panel.Join(toolBar, 1);

            bool showToolbar;
            try
            {
                showToolbar = ((bool) Settings.Default[toolBarName.Replace(" ", string.Empty)]);
            }
            catch (Exception)
            {
                showToolbar = false;
            }

            if (showToolbar)
            {
                ShowToolBar(toolBarName);
            }
            else
            {
                HideToolBar(toolBarName);
            }
        }

        public void HideToolBar(string toolbarName)
        {
            ToolStrip toolStrip;
            _toolBars.TryGetValue(toolbarName, out toolStrip);
            if (toolStrip == null)
            {
                throw new ArgumentException("Unregistered toolbar name");
            }

            toolStrip.Hide();

            if (OnToolBarVisibilityChanged != null)
            {
                OnToolBarVisibilityChanged(this, new ToolbarVisibilityChangedEventArgs(toolbarName, false));
            }
        }

        public void ShowToolBar(string toolbarName)
        {
            ToolStrip toolStrip;
            _toolBars.TryGetValue(toolbarName, out toolStrip);
            if (toolStrip == null)
            {
                throw new ArgumentException("Unregistered toolbar name");
            }

            toolStrip.Show();

            if (OnToolBarVisibilityChanged != null)
            {
                OnToolBarVisibilityChanged(this, new ToolbarVisibilityChangedEventArgs(toolbarName, true));
            }
        }
    }
}