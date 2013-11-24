using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
    class ToolBarService : IToolBarService
    {
        private readonly Dictionary<string, ToolStrip> _toolBars = new Dictionary<string, ToolStrip>();
        private ToolStripPanel _panel;

        public event EventHandler<ToolbarEventArgs> OnToolbarRegistered;
        public event EventHandler<ToolbarVisibilityChangedEventArgs> OnToolBarVisibilityChanged;

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

        public void DestroyService()
        {
        }

        public void InitService(params object[] objects)
        {
            if (objects.Length != 1)
            {
                throw new ArgumentException("Toolabar service expects exactly one param");
            }

            ToolStripPanel container = objects.First() as ToolStripPanel;
            if (container == null)
            {
                throw new ArgumentException("Expected first argumet to be a ToolStripPanel");
            }

            _panel = container;
        }
    }
}