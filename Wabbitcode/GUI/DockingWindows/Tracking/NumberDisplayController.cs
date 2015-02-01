using System;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    public abstract class NumberDisplayController<T> : AbstractVariableDisplayController<int, T>
    {
        private const int Base = 16;

        protected NumberDisplayController(ExpressionEvaluator evaluator) 
            : base(evaluator)
        {
        }

        protected override string GetDisplayValue(IWabbitcodeDebugger debugger, int address, int size)
        {
            return GetVarValue(debugger, address, size, Size);
        }

        protected override int ParseValidSize(string sizeString)
        {
            if (string.IsNullOrWhiteSpace(sizeString))
            {
                throw new ArgumentException("Null or empty size");
            }

            int outValue;
            if (!int.TryParse(sizeString, out outValue))
            {
                throw new ArgumentException("Size is not an integer");
            }

            return outValue;
        }

        private static string GetVarValue(IWabbitcodeDebugger debugger, int address, int size, int scale)
        {
            string outputValue = string.Empty;
            var values = debugger.ReadMemory((ushort) address, (ushort) (size * scale));

            for (int i = 0; i < size * scale; i += scale)
            {
                int total = 0;
                for (int j = 0; j < scale; j++)
                {
                    total += values[i + j] << (8 * j);
                }
                outputValue += Convert.ToString(total, Base).PadLeft(2 * scale, '0') + " ";
            }
            return outputValue;
        }
    }

    public class ByteDisplayController : NumberDisplayController<string>
    {
        public override string Name
        {
            get { return "Byte"; }
        }

        public ByteDisplayController(ExpressionEvaluator evaluator)
            : base(evaluator)
        {
        }

        public override string GetActualValue(IWabbitcodeDebugger debugger, int address, int size)
        {
            return GetDisplayValue(debugger, address, size);
        }

        public override int Size
        {
            get { return sizeof(byte); }
        }
    }

    public class WordDisplayController : NumberDisplayController<string>
    {
        public override string Name
        {
            get { return "Word"; }
        }

        public WordDisplayController(ExpressionEvaluator evaluator)
            : base(evaluator)
        {
        }

        public override string GetActualValue(IWabbitcodeDebugger debugger, int address, int size)
        {
            return GetDisplayValue(debugger, address, size);
        }

        public override int Size
        {
            get { return sizeof(ushort); }
        }
    }
}
