using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.GUI
{
    sealed class DebugToolBar : ToolStrip
    {
        private static readonly ComponentResourceManager Resources = new ComponentResourceManager(typeof(DebugToolBar));
        private readonly ToolStripButton _runDebuggerToolButton = new ToolStripButton
        {
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image) (Resources.GetObject("runDebuggerToolButton"))),
            Text = "Start Debug"
        };

        private readonly ToolStripButton _pauseToolButton = new ToolStripButton
        {
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Enabled = false,
            Image = ((Image) (Resources.GetObject("pauseToolButton"))),
            Text = "Pause"
        };

        private readonly ToolStripButton _stopDebugToolButton = new ToolStripButton
        {
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Enabled = false,
            Image = ((Image)(Resources.GetObject("stopToolButton"))),
            Text = "Stop"
        };

        private readonly ToolStripButton _restartToolStripButton = new ToolStripButton
        {
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Enabled = false,
            Image = ((Image) (Resources.GetObject("restartToolStripButton"))),
            Text = "Restart"
        };

        private readonly ToolStripSeparator _toolStripSeparator1 = new ToolStripSeparator();

        private readonly ToolStripButton _gotoCurrentToolButton = new ToolStripButton
        {
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Enabled = false,
            Image = ((Image) (Resources.GetObject("gotoCurrentToolButton"))),
            Text = "Goto Current Line"
        };

        private readonly ToolStripButton _stepToolButton = new ToolStripButton
        {
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Enabled = false,
            Image = ((Image) (Resources.GetObject("stepToolButton"))),
            Text = "Step"
        };

        private readonly ToolStripButton _stepOverToolButton = new ToolStripButton
        {
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Enabled = false,
            Image = ((Image) (Resources.GetObject("stepOverToolButton"))),
            Text = "Step Over"
        };

        private readonly ToolStripButton _stepOutToolButton = new ToolStripButton
        {
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Enabled = false,
            Image = ((Image) (Resources.GetObject("stepOutToolButton"))),
            Text = "Step Out"
        };

        private readonly IDebuggerService _debuggerService;
        private readonly IDocumentService _documentService;

        public DebugToolBar()
        {
            AllowItemReorder = true;
            GripStyle = ToolStripGripStyle.Hidden;
            Items.AddRange(new ToolStripItem[] {
            _runDebuggerToolButton,
            _pauseToolButton,
            _stopDebugToolButton,
            _restartToolStripButton,
            _toolStripSeparator1,
            _gotoCurrentToolButton,
            _stepToolButton,
            _stepOverToolButton,
            _stepOutToolButton});
            RenderMode = ToolStripRenderMode.System;

            _runDebuggerToolButton.Click += runDebuggerToolButton_Click;
            _pauseToolButton.Click += pauseToolButton_Click;
            _stopDebugToolButton.Click += stopDebugToolButton_Click;
            _restartToolStripButton.Click += restartToolStripButton_Click;
            _gotoCurrentToolButton.Click += gotoCurrentToolButton_Click;
            _stepToolButton.Click += stepToolButton_Click;
            _stepOverToolButton.Click += stepOverToolButton_Click;
            _stepOutToolButton.Click += stepOutToolButton_Click;

            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
            _documentService = ServiceFactory.Instance.GetServiceInstance<IDocumentService>();
            _debuggerService.OnDebuggingStarted += DebuggerService_OnDebuggingStarted;
        }

        private void DebuggerService_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            e.Debugger.OnDebuggerRunningChanged += (o, args) => EnableIcons();
            e.Debugger.OnDebuggerStep += (o, args) => EnableIcons();
        }

        private void EnableIcons()
        {
            if (InvokeRequired)
            {
                this.Invoke(EnableIcons);
                return;
            }

            bool isDebugging = _debuggerService.CurrentDebugger != null;
            bool isRunning = isDebugging && _debuggerService.CurrentDebugger.IsRunning;
            bool enabled = isDebugging && !isRunning;
            bool hasCallStack = isDebugging && _debuggerService.CurrentDebugger.CallStack.Count > 0;

            _gotoCurrentToolButton.Enabled = enabled;
            _stepToolButton.Enabled = enabled;
            _stepOverToolButton.Enabled = enabled;
            _stepOutToolButton.Enabled = enabled && hasCallStack;
            _stopDebugToolButton.Enabled = isDebugging;
            _restartToolStripButton.Enabled = isDebugging;
            _runDebuggerToolButton.Enabled = enabled || !isDebugging;
            _pauseToolButton.Enabled = isRunning;
        }

        private static void runDebuggerToolButton_Click(object sender, EventArgs e)
        {
            new StartDebuggerAction().Execute();
        }

        private static void pauseToolButton_Click(object sender, EventArgs e)
        {
            new PauseDebuggerAction().Execute();
        }

        private static void stopDebugToolButton_Click(object sender, EventArgs e)
        {
            new StopDebuggerAction().Execute();
        }

        private static void restartToolStripButton_Click(object sender, EventArgs e)
        {
            new RestartDebuggerAction().Execute();
        }

        private static void stepToolButton_Click(object sender, EventArgs e)
        {
            new StepDebuggerAction().Execute();
        }

        private static void stepOverToolButton_Click(object sender, EventArgs e)
        {
            new StepOverDebuggerAction().Execute();
        }

        private static void stepOutToolButton_Click(object sender, EventArgs e)
        {
            new StepOutDebuggerAction().Execute();
        }

        private void gotoCurrentToolButton_Click(object sender, EventArgs e)
        {
            _documentService.GotoCurrentDebugLine();
        }

    }
}
