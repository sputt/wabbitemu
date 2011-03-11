﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model.Types;
using System.Diagnostics;

namespace WabbitC.Model.Statements
{
	class Sub: ValueStatement
    {
        public Declaration LValue;
        public Datum SubValue;

        public Sub(Declaration lValue, Datum subValue)
        {
            LValue = lValue;
            SubValue = subValue;
        }

        public override string ToString()
        {
			StringBuilder sb = new StringBuilder(LValue.Name);
			sb.Append(" -= ");
			sb.Append(SubValue);
            sb.Append(";");
			return sb.ToString();
        }

		/// <summary>
		/// Builds the statements that generate the value of this expression
		/// </summary>
		/// <param name="block">Block to which to add the statements</param>
		/// <param name="expr">Expression to build</param>
		/// <param name="operand1">First operand to subtract</param>
		/// <param name="operand2">second operand to subtract to the first</param>
		/// <returns></returns>
		public static Declaration BuildStatements(Block block, Expression expr, Token operand1, Token operand2)
		{
			Debug.Assert(expr.Tokens[0].Text == "-");
			Debug.Assert(expr.Operands == 2);

			var decl = block.CreateTempDeclaration(TypeHelper.GetType(block, operand1));
			ValueStatement initialAssign = AssignmentHelper.ParseSingle(block, decl, operand1);
			block.Statements.Add(initialAssign);
			block.Statements.Add(new Sub(decl, Datum.Parse(block, operand2)));

			return decl;
		}
    }
}
