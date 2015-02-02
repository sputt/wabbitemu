using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    internal class EnumVariableDisplayController : ByteDisplayController
    {
        private readonly DebuggingEnum _enum;
        private readonly ISymbolService _symbolService;
        private readonly Dictionary<ByteMaskKey, string> _symbolCache = new Dictionary<ByteMaskKey, string>(); 

        public override string Name
        {
            get { return _enum.Name; }
        }

        public EnumVariableDisplayController(ExpressionEvaluator evaluator, DebuggingEnum debugEnum)
            : base(evaluator)
        {
            _enum = debugEnum;
            _symbolService = DependencyFactory.Resolve<ISymbolService>();
            var debuggerService = DependencyFactory.Resolve<IDebuggerService>();
            debuggerService.OnDebuggingStarted += _debuggerService_OnDebuggingStarted;
        }

        private void _debuggerService_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            _symbolCache.Clear();
            foreach (var maskKey in _enum.EnumMapping)
            {
                foreach (var enumVal in maskKey.Value)
                {
                    var addressFromLabel = _symbolService.SymbolTable.GetAddressFromLabel(enumVal);
                    if (addressFromLabel == null)
                    {
                        continue;
                    }

                    byte enumByteVal = (byte) addressFromLabel;
                    _symbolCache.Add(new ByteMaskKey(enumByteVal, maskKey.Key), enumVal);
                }
            }
        }

        protected override string GetDisplayValue(IWabbitcodeDebugger debugger, int address, int size)
        {
            byte readByte = debugger.ReadByte((ushort) address);

            var enumValues = new List<string>();
            foreach (var maskKey in _enum.EnumMapping)
            {
                string label;
                byte maskedValue = (byte) (readByte & maskKey.Key);
                if (_symbolCache.TryGetValue(new ByteMaskKey(maskedValue, maskKey.Key), out label))
                {
                    enumValues.Add(label);
                }
            }

            if (enumValues.Count == 0)
            {
                throw new ArgumentException("Invalid enum type: " + readByte);
            }

            return string.Join(" | ", enumValues);
        }
    }

    internal class ByteMaskKey
    {
        private readonly byte _byte;
        private readonly byte _mask;

        public ByteMaskKey(byte byteValue, byte maskValue)
        {
            _byte = byteValue;
            _mask = maskValue;
        }

        public override int GetHashCode()
        {
            return (_byte << 8) + _mask;
        }

        public override bool Equals(object obj)
        {
            ByteMaskKey key = obj as ByteMaskKey;
            if (key == null)
            {
                return false;
            }

            return key._byte == _byte && key._mask == _mask;
        }
    }
}