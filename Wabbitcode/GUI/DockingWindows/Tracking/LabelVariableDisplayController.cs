using System;
using System.Linq;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    internal class LabelVariableDisplayController : AbstractVariableDisplayController<int, string>
    {
        private readonly ISymbolService _symbolService;

        public LabelVariableDisplayController(ExpressionEvaluator evaluator) : base(evaluator)
        {
            _symbolService = DependencyFactory.Resolve<ISymbolService>();
        }

        public override string Name
        {
            get { return "Label"; }
        }

        public override int Size
        {
            get { return sizeof(ushort); }
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

        protected override string GetDisplayValue(IWabbitcodeDebugger debugger, int address, int size)
        {
            var data = debugger.ReadShort((ushort) address);
            var symbolList = _symbolService.SymbolTable.GetLabelsFromAddress(data);
            return symbolList.Count == 0 ? data.ToString("X") : symbolList.First();
        }

        public override string GetActualValue(IWabbitcodeDebugger debugger, int address, int size)
        {
            return GetDisplayValue(debugger, address, size);
        }
    }
}