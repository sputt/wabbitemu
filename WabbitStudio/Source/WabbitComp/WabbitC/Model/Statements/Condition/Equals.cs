using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements.Condition
{
    class Equals : ConditionStatement, IMathOperator
    {
        public Equals()
        {
        }

        public Equals(Declaration lValue, Declaration condDecl, Datum condValue)
        {
            LValue = lValue;
            CondDecl = condDecl;
            Operator = Tokenizer.ToToken("==");
            CondValue = condValue;
        }

		public override Token GetInverseOperator()
		{
			return Tokenizer.ToToken("!=");
		}

        public override string ToAssemblyString()
        {
            string equals = "or a";
            //TODO: make sure condDecl = __hl
            return equals + "\tsbc __hl, " + CondDecl.ToString(); 
        }

        #region IMathOperator Members

        public Token GetHandledOperator()
        {
            return Tokenizer.ToToken("==");
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
