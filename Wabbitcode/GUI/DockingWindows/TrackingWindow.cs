using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.GUI.DockingWindows.Tracking;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class TrackingWindow : ToolWindow
    {
        private readonly List<BitmapViewer> _openBitmaps = new List<BitmapViewer>();
        private readonly TrackingTreeModel _model;
        private readonly IDebuggerService _debuggerService;

        private IWabbitcodeDebugger _debugger;
        private TrackingVariableRowModel _emptyRowModel;

        public TrackingWindow()
        {
            InitializeComponent();

            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
            _debuggerService.OnDebuggingStarted += DebuggerService_OnDebuggingStarted;
            _debuggerService.OnDebuggingEnded += DebuggerService_OnDebuggingEnded;
            _valueTypeBox.DropDownItems.Clear();
            _valueTypeBox.DropDownItems.AddRange(VariableDisplayManager.Instance.ControllerNames);

            // TODO: fix
            _model = new TrackingTreeModel();
            AddEmptyRow();
            variablesDataView.Model = _model;
            EnablePanel(false);
        }

        private void DebuggerService_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            _debugger = e.Debugger;
            _debugger.DebuggerStep += OnDebuggerOnDebuggerStep;
            _debugger.DebuggerRunningChanged += OnDebuggerOnDebuggerRunningChanged;
        }

        private void DebuggerService_OnDebuggingEnded(object sender, DebuggingEventArgs e)
        {
            _debugger.DebuggerStep -= OnDebuggerOnDebuggerStep;
            _debugger.DebuggerRunningChanged -= OnDebuggerOnDebuggerRunningChanged;
            _debugger = null;
            EnablePanel(false);
            foreach (var viewer in _openBitmaps)
            {
                viewer.Close();
            }
        }

        private void OnDebuggerOnDebuggerRunningChanged(object o, DebuggerRunningEventArgs args)
        {
            if (!args.Running)
            {
                UpdateAllRows();
            }

            EnablePanel(!args.Running);
        }

        private void OnDebuggerOnDebuggerStep(object o, DebuggerStepEventArgs args)
        {
            UpdateAllRows();
            EnablePanel(true);
        }

        protected override sealed void EnablePanel(bool enabled)
        {
            base.EnablePanel(enabled);

            variablesDataView.Enabled = enabled;
        }

        #region Clipboard Operation

        public override void Copy()
        {
            if (variablesDataView == null)
            {
                return;
            }

            if (variablesDataView.CurrentEditor != null)
            {
                Clipboard.SetData(DataFormats.Text, variablesDataView.CurrentEditor.Text);
            }
        }

        public override void Paste()
        {
            if (variablesDataView.CurrentEditor != null)
            {
                variablesDataView.CurrentEditor.Text = Clipboard.GetData(DataFormats.Text).ToString();
            }
        }

        #endregion

        #region Row Data


        private void ValueTypeBox_OnChangesApplied(object sender, EventArgs eventArgs)
        {
            var model = (TrackingVariableRowModel)variablesDataView.CurrentNode.Tag;
            model.IsCacheValid = false;
            _model.OnNodesChanged(model);
        }

        private void AddressBoxOnChangesApplied(object sender, EventArgs eventArgs)
        {
            var model = (TrackingVariableRowModel)variablesDataView.CurrentNode.Tag;
            model.IsCacheValid = false;
            _model.OnNodesChanged(model);

            if (!string.IsNullOrEmpty(_emptyRowModel.Address))
            {
                AddEmptyRow();
            }
        }

        private void NumBytesBox_OnChangesApplied(object sender, EventArgs eventArgs)
        {
            var model = (TrackingVariableRowModel)variablesDataView.CurrentNode.Tag;
            model.IsCacheValid = false;
            _model.OnNodesChanged(model);

            if (!string.IsNullOrEmpty(_emptyRowModel.Address))
            {
                AddEmptyRow();
            }
        }

        private void AddEmptyRow()
        {
            _emptyRowModel = new TrackingVariableRowModel(_debuggerService, VariableDisplayManager.Instance);
            _model.Nodes.Add(_emptyRowModel);
            _model.OnStructureChanged();
        }

        #endregion

        private void UpdateAllRows()
        {
            foreach (var model in _model.Nodes)
            {
                model.IsCacheValid = false;
                var recalcedValue = model.Value;
                if (recalcedValue == null)
                {
                    throw new Exception("Invalid value");
                }
            }

            if (InvokeRequired)
            {
                this.Invoke(() => _model.OnStructureChanged());
            }
            else
            {
                _model.OnStructureChanged();
            }
        }

        private void temp_FormClosed(object sender, FormClosedEventArgs e)
        {
            foreach (BitmapViewer test in _openBitmaps.Where(test => test.Tag == ((Form) sender).Tag))
            {
                _openBitmaps.Remove(test);
                break;
            }
        }

        private void variablesDataView_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode != Keys.Delete)
            {
                return;
            }

            var currentModel = (TrackingVariableRowModel)variablesDataView.CurrentNode.Tag;
            if (currentModel == _emptyRowModel)
            {
                return;
            }

            _model.Nodes.Remove(currentModel);
            _model.OnNodesRemoved(currentModel);
        }

        private void variablesDataView_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            var node = variablesDataView.GetNodeAt(e.Location);
            if (node == null)
            {
                return;
            }

            var model = node.Tag as TrackingVariableRowModel;
            if (model == null)
            {
                return;
            }

            // TODO: abstract with the controller
            /*if (model.ValueImage != null)
            {
                BitmapViewer temp = new BitmapViewer(model.ValueImage)
                {
                    Tag = model.ValueImage
                };

                temp.Show();
                _openBitmaps.Add(temp);
                temp.FormClosed += temp_FormClosed;
            }*/
        }
    }
}