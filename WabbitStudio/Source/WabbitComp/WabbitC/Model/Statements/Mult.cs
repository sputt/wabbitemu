using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model.Types;
using System.Diagnostics;

namespace WabbitC.Model.Statements
{
    class Mult : ValueStatement
    {
        public Declaration LValue;
        public Datum MultValue;

        public Mult(Declaration lValue, Datum addValue)
        {
            LValue = lValue;
            MultValue = addValue;
        }

        public override string ToString()
        {
			StringBuilder sb = new StringBuilder(LValue.Name);
			sb.Append(" *= ");
			sb.Append(MultValue);
            sb.Append(";");
			return sb.ToString();
        }

		/// <summary>
		/// Builds the statements that generate the value of this expression
		/// </summary>
		/// <param name="block">Block to which to add the statements</param>
		/// <param name="expr">Expression to build</param>
		/// <param name="operand1">First operand to multiply</param>
		/// <param name="operand2">second operand to multiply to the first</param>
		/// <returns></returns>
		public static Declaration BuildStatements(Block block, Expression expr, Token operand1, Token operand2)
		{
			Debug.Assert(expr.Tokens[0].Text == "*");
			Debug.Assert(expr.Operands == 2);

			var decl = block.CreateTempDeclaration(TypeHelper.GetType(block, operand1));
			ValueStatement initialAssign = AssignmentHelper.ParseSingle(block, decl, operand1);
			block.Statements.Add(initialAssign);
			block.Statements.Add(new Mult(decl, Datum.Parse(block, operand2)));

			return decl;
		}
    }
}
