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
            LValue.ConstValue.Value = (LValue.ConstValue.Value + (RValue as Immediate).Value).Eval()[0].Token;
            return LValue.ConstValue;
        }

        #endregion
    }
}
