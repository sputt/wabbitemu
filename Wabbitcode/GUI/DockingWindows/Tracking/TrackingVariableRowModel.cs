using System;
using System.Collections.Generic;
using System.Linq;
using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    public class TrackingVariableRowModel : IDisposable
    {
        private readonly IDebuggerService _debuggerService;
        private readonly VariableDisplayManager _variableDisplayManager;

        protected IVariableDisplayController DisplayController;
        protected IWabbitcodeDebugger Debugger;
        protected string CachedValue;
        private bool _disposed;

        #region Properties

        internal int ChildSize { get; private set; }

        public bool IsCacheValid { private get; set; }

        [UsedImplicitly]
        public string Address { get; set; }

        [UsedImplicitly]
        public string NumBytesString { get; set; }

        [UsedImplicitly]
        public string ValueType
        {
            get { return DisplayController.Name; }
            set { DisplayController = _variableDisplayManager.GetVariableDisplayController(value); }
        }

        [UsedImplicitly]
        public string Value
        {
            get
            {
                return IsCacheValid ? CachedValue : CacheValue();
            }
        }

        protected virtual string CacheValue()
        {
            try
            {
                CachedValue = DisplayController.GetDisplayValue(Debugger, Address, NumBytesString);
                IsCacheValid = true;
                return CachedValue;
            }
            catch (Exception ex)
            {
                return ex.Message;
            }
        }

        public IEnumerable<TrackingVariableRowModel> Children
        {
            get
            {
                var compositeController = DisplayController as CompositeVariableDisplayController;
                if (compositeController == null)
                {
                    return new List<TrackingVariableRowModel>();
                }

                var offset = 0;
                var offsetArray = new int[compositeController.DebuggingStructure.Properties.Count];
                var children = compositeController.ChildControllers.ToList();
                for (int i = 0; i < compositeController.DebuggingStructure.Properties.Count; i++)
                {
                    offsetArray[i] = offset;
                    offset += children[i].Size;
                }

                ChildSize = offset;
                offset = 0;
                return compositeController.DebuggingStructure.Properties.Select(p =>
                    new ChildTrackingVariableRowModel(_debuggerService, _variableDisplayManager, this, offsetArray[offset++])
                {
                    Address = p.Name,
                    NumBytesString = p.Size.ToString(),
                    ValueType = p.Controller.Name
                });
            }
        }

        #endregion

        public TrackingVariableRowModel(IDebuggerService debuggerService, VariableDisplayManager variableDisplayManager)
        {
            _debuggerService = debuggerService;
            _variableDisplayManager = variableDisplayManager;
            DisplayController = variableDisplayManager.DefaultController;
            Debugger = _debuggerService.CurrentDebugger;

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
            Debugger = e.Debugger;
        }

        private void DebuggerService_OnDebuggingEnded(object sender, DebuggingEventArgs e)
        {
            Debugger = null;
        }
    }

    class ChildTrackingVariableRowModel : TrackingVariableRowModel
    {
        private readonly TrackingVariableRowModel _parent;
        private readonly int _offset;

        public ChildTrackingVariableRowModel(IDebuggerService debuggerService, VariableDisplayManager variableDisplayManager, 
            TrackingVariableRowModel parent, int offset)
            : base(debuggerService, variableDisplayManager)
        {
            _offset = offset;
            _parent = parent;
        }

        protected override string CacheValue()
        {
            try
            {
                // Generate an string that looks like:
                // baseAddress + (offset * structSize) + childOffset
                var addressString = _parent.Address + "+(" + _parent.NumBytesString + "*" + _parent.ChildSize + ")+" +
                                    _offset;
                CachedValue = DisplayController.GetDisplayValue(Debugger, addressString, NumBytesString);
                IsCacheValid = true;
                return CachedValue;
            }
            catch (Exception ex)
            {
                return ex.Message;
            }
        }
    }
}