using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using WabbitC.Model.Types;

namespace WabbitC.Model.Statements.Math
{
    class Add : MathStatement, IMathOperator
    {
        public Add()
        {
        }

        public Add(Declaration lValue, Datum rValue)
        {
            LValue = lValue;
            Operator = Token.AddOperatorToken;
            RValue = rValue;
        }

        #region IMathOperator Members

        public Token GetHandledOperator()
        {
            return Token.AddOperatorToken;
        }

        Immediate IMathOperator.Apply()
        {
			var imm = RValue as Immediate;
			if (imm == null)
				return null;
			LValue.ConstValue = LValue.ConstValue + imm;
            return LValue.ConstValue;
        }

        #endregion

		public override string ToAssemblyString()
		{
			return "add " + LValue.Name + "," + RValue.ToString();
		}
    }
}
