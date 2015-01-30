using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    public interface IVariableDisplayController
    {
        string GetDisplayValue(IWabbitcodeDebugger debugger, string address, string size);
        string Name { get; }
    }

    public abstract class AbstractVariableDisplayController<TSize, TReturnValue> : IVariableDisplayController
    {
        protected readonly ExpressionEvaluator Evaluator;

        public abstract string Name { get; }

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

        public abstract TReturnValue GetActualValue(IWabbitcodeDebugger debugger, int address, TSize size);

        public string GetDisplayValue(IWabbitcodeDebugger debugger, string address, string size)
        {
            return GetDisplayValue(debugger, ParseAddress(address), ParseValidSize(size));
        }
    }
}
