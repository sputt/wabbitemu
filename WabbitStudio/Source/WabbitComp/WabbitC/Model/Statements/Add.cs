using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
    class Add : ValueStatement
    {
        public Declaration LValue;
        public Datum AddValue;

        public Add(Declaration lValue, Datum addValue)
        {
            LValue = lValue;
            AddValue = addValue;
        }

        public override string ToString()
        {
			StringBuilder sb = new StringBuilder(LValue.Name);
			sb.Append(" += ");
			sb.Append(AddValue);
            sb.Append(";");
			return sb.ToString();
        }

        /// <summary>
        /// Builds the statements that generate the value of this expression
        /// </summary>
        /// <param name="block">Block to which to add the statements</param>
        /// <param name="stack">Stack of parameters for this expression</param>
        /// <param name="expr">Expression to build</param>
        /// <returns></returns>
        public static Declaration BuildStatements(Block block, Stack<Token> stack, Expression expr)
        {
            Debug.Assert(expr.Tokens[0].Text == "+");
            Debug.Assert(expr.Operands == 2);

            var arg1 = stack.Pop();
            var arg2 = stack.Pop();
            var decl = block.CreateTempDeclaration(TypeHelper.GetType(block, arg1));
            ValueStatement initialAssign = AssignmentHelper.ParseSingle(block, decl, arg1);
            block.Statements.Add(initialAssign);
            block.Statements.Add(new Add(decl, Datum.Parse(block, arg2)));

            return decl;
        }
    }
}
