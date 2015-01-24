using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public enum VariableDisplayMethod
    {
        BlackWhiteImage,
        GrayscaleImage,
        Binary,
        Octal,
        Decimal,
        Hexadecimal,
        String,
    }

    public enum VariableType
    {
        Byte,
        Word,
        Image,
        String,
    }

    public partial class TrackingWindow : ToolWindow
    {
        private readonly List<BitmapViewer> _openBitmaps = new List<BitmapViewer>();
        private readonly TrackingTreeModel _model;
        private readonly IDebuggerService _debuggerService;
        private readonly ExpressionEvaluator _expressionEvaluator;

        private IWabbitcodeDebugger _debugger;
        private TrackingVariableRowModel _emptyRowModel;

        public TrackingWindow()
        {
            InitializeComponent();

            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
            ISymbolService symbolService = DependencyFactory.Resolve<ISymbolService>();
            _debuggerService.OnDebuggingStarted += DebuggerService_OnDebuggingStarted;
            _debuggerService.OnDebuggingEnded += DebuggerService_OnDebuggingEnded;
            _expressionEvaluator = new ExpressionEvaluator(symbolService, _debuggerService);

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
            _displayMethodBox.DropDownItems.Clear();
            TrackingVariableRowModel currentModel = (TrackingVariableRowModel) variablesDataView.CurrentNode.Tag;
            switch (currentModel.ValueType)
            {
                case VariableType.Byte:
                case VariableType.Word:
                    _displayMethodBox.DropDownItems.Add(VariableDisplayMethod.Hexadecimal);
                    _displayMethodBox.DropDownItems.Add(VariableDisplayMethod.Decimal);
                    _displayMethodBox.DropDownItems.Add(VariableDisplayMethod.Binary);
                    _displayMethodBox.DropDownItems.Add(VariableDisplayMethod.Octal);
                    currentModel.DisplayMethod = VariableDisplayMethod.Hexadecimal;
                    break;
                case VariableType.Image:
                    _displayMethodBox.DropDownItems.Add(VariableDisplayMethod.BlackWhiteImage);
                    _displayMethodBox.DropDownItems.Add(VariableDisplayMethod.GrayscaleImage);
                    currentModel.DisplayMethod = VariableDisplayMethod.BlackWhiteImage;
                    break;
                case VariableType.String:
                    _displayMethodBox.DropDownItems.Add(VariableDisplayMethod.String);
                    currentModel.DisplayMethod = VariableDisplayMethod.String;
                    break;
                default:
                    throw new Exception("Invalid var type");
            }

            _model.OnNodesChanged((TrackingVariableRowModel)variablesDataView.CurrentNode.Tag);
        }

        private void AddressBoxOnChangesApplied(object sender, EventArgs eventArgs)
        {
            _model.OnNodesChanged((TrackingVariableRowModel)variablesDataView.CurrentNode.Tag);

            if (!string.IsNullOrEmpty(_emptyRowModel.Address))
            {
                AddEmptyRow();
            }
        }

        private void NumBytesBox_OnChangesApplied(object sender, EventArgs eventArgs)
        {
            _model.OnNodesChanged((TrackingVariableRowModel)variablesDataView.CurrentNode.Tag);

            if (!string.IsNullOrEmpty(_emptyRowModel.Address))
            {
                AddEmptyRow();
            }
        }

        private void AddEmptyRow()
        {
            _emptyRowModel = new TrackingVariableRowModel(_debuggerService, _expressionEvaluator);
            _model.Nodes.Add(_emptyRowModel);
            _model.OnStructureChanged();
        }

        private void DisplayMethodBox_OnChangesApplied(object sender, EventArgs eventArgs)
        {
            _model.OnNodesChanged((TrackingVariableRowModel)variablesDataView.CurrentNode.Tag);
        }

        #endregion

        private void UpdateAllRows()
        {
            foreach (var model in _model.Nodes)
            {
                model.IsCachedValueValid = false;
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

            if (model.ValueImage != null)
            {
                BitmapViewer temp = new BitmapViewer(model.ValueImage)
                {
                    Tag = model.ValueImage
                };

                temp.Show();
                _openBitmaps.Add(temp);
                temp.FormClosed += temp_FormClosed;
            }
        }
    }
}