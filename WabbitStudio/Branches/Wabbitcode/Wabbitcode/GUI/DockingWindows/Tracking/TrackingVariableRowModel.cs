using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    public class TrackingVariableRowModel : IDisposable
    {
        private readonly IDebuggerService _debuggerService;
        private readonly List<TrackingVariableRowModel> _children = new List<TrackingVariableRowModel>();
        private readonly VariableDisplayManager _variableDisplayManager;

        private IWabbitcodeDebugger _debugger;
        private bool _disposed;
        private IVariableDisplayController _displayController;
        private string _cachedValue;

        #region Properties

        public bool IsCacheValid { get; set; }

        [UsedImplicitly]
        public string Address { get; set; }

        [UsedImplicitly]
        public string NumBytesString { get; set; }

        [UsedImplicitly]
        public string ValueType
        {
            get { return _displayController.Name; }
            set { _displayController = _variableDisplayManager.GetVariableDisplayController(value); }
        }

        [UsedImplicitly]
        public string Value
        {
            get
            {
                return IsCacheValid ? _cachedValue : CacheValue();
            }
        }

        private string CacheValue()
        {
            try
            {
                _cachedValue = _displayController.GetDisplayValue(_debugger, Address, NumBytesString);
                IsCacheValid = true;
                return _cachedValue;
            }
            catch (Exception ex)
            {
                return ex.Message;
            }
        }

        public List<TrackingVariableRowModel> Children { get { return _children; } }

        #endregion

        public TrackingVariableRowModel(IDebuggerService debuggerService, VariableDisplayManager variableDisplayManager)
        {
            _debuggerService = debuggerService;
            _variableDisplayManager = variableDisplayManager;
            _displayController = variableDisplayManager.DefaultController;
            _debugger = _debuggerService.CurrentDebugger;

            _debuggerService.OnDebuggingStarted += DebuggerService_OnDebuggingStarted;
            _debuggerService.OnDebuggingEnded += DebuggerService_OnDebuggingEnded;
        }

        ~TrackingVariableRowModel()
        {
           Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
        }

        private void Dispose(bool disposing)
        {
            if (_disposed)
            {
                return;
            }

            if (disposing)
            {
                _debuggerService.OnDebuggingStarted -= DebuggerService_OnDebuggingStarted;
                _debuggerService.OnDebuggingEnded -= DebuggerService_OnDebuggingEnded;
            }

            _disposed = true;
        }

        private void DebuggerService_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            _debugger = e.Debugger;
        }

        private void DebuggerService_OnDebuggingEnded(object sender, DebuggingEventArgs e)
        {
            _debugger = null;
        }
    }
}