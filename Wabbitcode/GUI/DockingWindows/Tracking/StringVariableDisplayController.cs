using System;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    class StringVariableDisplayController : AbstractVariableDisplayController<int, string>
    {
        private const int ReadToEnd = -1;
        public StringVariableDisplayController(ExpressionEvaluator evaluator) : base(evaluator)
        {
        }

        public override string Name
        {
            get { return "String"; }
        }

        protected override int ParseValidSize(string sizeString)
        {
            int resultInt;
            if (!int.TryParse(sizeString, out resultInt))
            {
                throw new ArgumentException("Invalid size");
            }

            if (resultInt < ReadToEnd)
            {
                throw new ArgumentException("Size must be >= -1");
            }

            return resultInt;
        }

        protected override string GetDisplayValue(IWabbitcodeDebugger debugger, int address, int size)
        {
            string outputValue = string.Empty;
            if (size == ReadToEnd)
            {
                int i = 0;
                char charToAdd;
                do
                {
                    charToAdd = (char)debugger.ReadByte((ushort)(address + i));
                    outputValue += charToAdd.ToString();
                    i++;
                } while (charToAdd != '\0');
            }
            else
            {
                var bytes = debugger.ReadMemory((ushort) address, (ushort) size);
                for (int i = 0; i < size; i++)
                {
                    outputValue += (char) bytes[i];
                }
            }

            return outputValue;
        }

        public override string GetActualValue(IWabbitcodeDebugger debugger, int address, int size)
        {
            return GetDisplayValue(debugger, address, size);
        }
    }
}