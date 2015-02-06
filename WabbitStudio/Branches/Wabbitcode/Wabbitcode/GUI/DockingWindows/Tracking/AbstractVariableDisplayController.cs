using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    public abstract class AbstractVariableDisplayController<TSize> : IVariableDisplayController
    {
        protected readonly ExpressionEvaluator Evaluator;

        public abstract string Name { get; }

        public abstract int Size { get; }

        protected AbstractVariableDisplayController(ExpressionEvaluator evaluator)
        {
            Evaluator = evaluator;
        }

        protected virtual int ParseAddress(string addressString)
        {
            var tokenList = ExpressionEvaluator.CreateTokenList(addressString);
            var postFix = ExpressionEvaluator.InfixToPostfix(tokenList);
            int value = Evaluator.EvalPostfix(postFix);
            return value;
        }

        protected abstract TSize ParseValidSize(string sizeString);

        protected abstract string GetDisplayValue(IWabbitcodeDebugger debugger, int address, TSize size);

        protected abstract object GetActualValue(IWabbitcodeDebugger debugger, int address, TSize size);

        public string GetDisplayValue(IWabbitcodeDebugger debugger, string address, string size)
        {
            return GetDisplayValue(debugger, ParseAddress(address), ParseValidSize(size));
        }

        public object GetActualValue(IWabbitcodeDebugger debugger, string address, string size)
        {
            return GetActualValue(debugger, ParseAddress(address), ParseValidSize(size));
        }
    }
}
