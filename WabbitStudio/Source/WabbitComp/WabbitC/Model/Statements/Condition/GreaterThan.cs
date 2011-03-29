using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements.Condition
{
    class GreaterThan : ConditionStatement, IMathOperator
    {
        public GreaterThan()
        {
        }

        public GreaterThan(Declaration lValue, Declaration condDecl, Datum condValue)
        {
            LValue = lValue;
            CondDecl = condDecl;
            Operator = Tokenizer.ToToken(">");
            CondValue = condValue;
        }

        #region IMathOperator Members

        public Token GetHandledOperator()
        {
            return Tokenizer.ToToken(">");
        }

        Immediate IMathOperator.Apply()
        {
            //LValue.ConstValue.Value = (LValue.ConstValue.Value * (RValue as Immediate).Value).Eval()[0].Token;
            //return LValue.ConstValue;
            throw new NotImplementedException();
        }

        #endregion
    }
}
