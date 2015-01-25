using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Utils
{
    public class ExpressionEvaluator
    {
        private readonly IDebuggerService _debuggerService;
        private readonly ISymbolService _symbolService;

        public ExpressionEvaluator(ISymbolService symbolService, IDebuggerService debuggerService)
        {
            _symbolService = symbolService;
            _debuggerService = debuggerService;
        }

        private static bool Predecessor(string firstOperator, string secondOperator)
        {
            const string opString = "(+-*/%";

            int[] precedence = { 0, 12, 12, 13, 13, 13 };

            int firstPoint = opString.IndexOf(firstOperator, StringComparison.Ordinal);
            int secondPoint = opString.IndexOf(secondOperator, StringComparison.Ordinal);

            return precedence[firstPoint] >= precedence[secondPoint];
        }

        public static IEnumerable<string> InfixToPostfix(IEnumerable<string> infixArray)
        {
            var stack = new Stack<string>();
            var postfix = new Stack<string>();

            foreach (string op in infixArray)
            {
                if (!("()*/+-%".Contains(op)))
                {
                    postfix.Push(op);
                }
                else
                {
                    if (op.Equals("("))
                    {
                        stack.Push("(");
                    }
                    else
                    {
                        string st;
                        if (op.Equals(")"))
                        {
                            st = stack.Pop();
                            while (!(st.Equals("(")))
                            {
                                postfix.Push(st);
                                st = stack.Pop();
                            }
                        }
                        else
                        {
                            while (stack.Count > 0)
                            {
                                st = stack.Pop();
                                if (Predecessor(st, op))
                                {
                                    postfix.Push(st);
                                }
                                else
                                {
                                    stack.Push(st);
                                    break;
                                }
                            }
                            stack.Push(op);
                        }
                    }
                }
            }
            while (stack.Count > 0)
            {
                postfix.Push(stack.Pop());
            }

            return new Stack<string>(postfix);
        }

        private int EvaluateElement(string element)
        {
            if (!Settings.Default.CaseSensitive)
            {
                element = element.ToUpper();
            }

            int value;
            if (int.TryParse(element, out value))
            {
                return value;
            }

            string label = _symbolService.SymbolTable.GetAddressFromLabel(element);
            if (label != null)
            {
                return int.Parse(label, NumberStyles.HexNumber);
            }

            IWabbitcodeDebugger debugger = _debuggerService.CurrentDebugger;
            if (debugger == null)
            {
                return 0;
            }

            switch (element)
            {
                case "$A":
                    return debugger.CPU.A;
                case "$F":
                    return debugger.CPU.F;
                case "$B":
                    return debugger.CPU.B;
                case "$C":
                    return debugger.CPU.C;
                case "$D":
                    return debugger.CPU.D;
                case "$E":
                    return debugger.CPU.E;
                case "$H":
                    return debugger.CPU.H;
                case "$L":
                    return debugger.CPU.L;
                case "$IXH":
                    return debugger.CPU.IXH;
                case "$IXL":
                    return debugger.CPU.IXL;
                case "$IYH":
                    return debugger.CPU.IYH;
                case "$IYL":
                    return debugger.CPU.IYL;
                case "$AF":
                    return debugger.CPU.AF;
                case "$BC":
                    return debugger.CPU.BC;
                case "$DE":
                    return debugger.CPU.DE;
                case "$HL":
                    return debugger.CPU.HL;
                case "$IX":
                    return debugger.CPU.IX;
                case "$IY":
                    return debugger.CPU.IY;
                case "$PC":
                    return debugger.CPU.PC;
                case "$SP":
                    return debugger.CPU.SP;
                default:
                    if (element.StartsWith("$") || element.EndsWith("h"))
                    {
                        element = element.Replace('$', ' ').Replace('h', ' ').Trim();
                        if (!int.TryParse(element, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value))
                        {
                            break;
                        }
                        return value;
                    }
                    break;
            }
            throw new FormatException(string.Format("Can't recognize symbol {0}", element));
        }

        private int EvaluateElements(string right, string left, string op)
        {
            int leftVal = EvaluateElement(right);
            int rightVal = EvaluateElement(left);
            switch (op)
            {
                case "+":
                    return leftVal + rightVal;
                case "-":
                    return leftVal - rightVal;
                case "*":
                    return leftVal * rightVal;
                case "/":
                    return leftVal / rightVal;
                case "%":
                    return leftVal % rightVal;
            }
            throw new Exception("Invalid operator");
        }

        public int EvalPostfix(IEnumerable<string> postFix)
        {
            Stack<string> parseStack = new Stack<string>();
            foreach (string element in postFix)
            {
                if (element == "+" || element == "-" || element == "*" || element == "/")
                {
                    if (parseStack.Count < 2)
                    {
                        throw new FormatException("Invalid syntax");
                    }

                    string left = parseStack.Pop();
                    string right = parseStack.Pop();
                    int eval = EvaluateElements(right, left, element);
                    parseStack.Push(eval.ToString());
                }
                else
                {
                    parseStack.Push(element);
                }
            }

            return parseStack.Count == 0 ? 0 : EvaluateElement(parseStack.Pop());
        }

        public static IEnumerable<string> CreateTokenList(string expression)
        {
            Regex re = new Regex(@"([\+\-\*\(\)\^\/\ ])", RegexOptions.Compiled);
            if (string.IsNullOrEmpty(expression))
            {
                throw new FormatException("Missing address value");
            }

            var tokenList = re.Split(expression).Select(t => t.Trim()).Where(t => !string.IsNullOrEmpty(t));
            return tokenList;
        }
    }
}
