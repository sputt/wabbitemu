using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements.Math
{
    class Not : MathStatement, IMathOperator
    {
        public Not()
        {
        }

        public Not(Declaration lValue)
        {
            LValue = lValue;
            Operator = Token.NOTOperatorToken;
            RValue = null;
        }

        #region IMathOperator Members

        public Token GetHandledOperator()
        {
            return Token.NOTOperatorToken;
        }

        Immediate IMathOperator.Apply()
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}
