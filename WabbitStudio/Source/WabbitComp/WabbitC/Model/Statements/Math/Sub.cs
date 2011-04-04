using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model.Types;
using System.Diagnostics;

namespace WabbitC.Model.Statements.Math
{
	class Sub: MathStatement, IMathOperator
    {
        public Sub()
        {
        }

        public Sub(Declaration lValue, Datum subValue)
        {
            LValue = lValue;
            Operator = Token.SubOperatorToken;
            RValue = subValue;
        }

        #region IMathOperator Members

        public Token GetHandledOperator()
        {
            return Token.SubOperatorToken;
        }

        Immediate IMathOperator.Apply()
        {
			var imm = RValue as Immediate;
			if (imm == null)
				return null;
			LValue.ConstValue.Value = (LValue.ConstValue.Value - imm.Value).Eval()[0].Token;
			return LValue.ConstValue;
        }

        #endregion

		public override string ToAssemblyString()
		{
			return "add " + LValue.Name + "," + RValue.ToString();
		}
    }
}
