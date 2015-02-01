using System;
using System.Collections.Generic;
using System.Linq;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    internal class EnumVariableDisplayController : ByteDisplayController
    {
        private readonly DebuggingEnum _enum;

        public override string Name
        {
            get { return _enum.Name; }
        }

        public EnumVariableDisplayController(ExpressionEvaluator evaluator, DebuggingEnum debugEnum)
            : base(evaluator)
        {
            _enum = debugEnum;
        }

        protected override string GetDisplayValue(IWabbitcodeDebugger debugger, int address, int size)
        {
            byte readByte = debugger.ReadByte((ushort) address);

            var enumValues = new List<string>();
            foreach (var enumVal in _enum.EnumMapping)
            {
                if ((readByte & enumVal.Key.Mask) == enumVal.Key.Byte)
                {
                    enumValues.Add(enumVal.Value);
                }
            }

            if (enumValues.Count == 0)
            {
                throw new ArgumentException("Invalid enum type: " + readByte);
            }

            return string.Join(" | ", enumValues);
        }
    }

    internal class EnumByteKey
    {
        public byte Byte { get; private set; }

        public byte Mask { get; private set; }

        public EnumByteKey(byte byteValue, byte maskValue)
        {
            Byte = byteValue;
            Mask = maskValue;
        }
    }
}