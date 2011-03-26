using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
    class ConditionStatement : ValueStatement
    {
        public Declaration LValue;
        public Declaration CondDecl;
        public Token Operator;
        public Datum CondValue;

        public static Declaration BuildStatements(Block block, Expression expr, Token operand1, Token operand2)
        {
            Token opToken = expr.Tokens[0];

            var decl = block.CreateTempDeclaration(TypeHelper.GetType(block, operand1));
            ValueStatement initialAssign = AssignmentHelper.ParseSingle(block, decl, operand1);
            block.Statements.Add(initialAssign);

            var asm = System.Reflection.Assembly.GetExecutingAssembly();
            var objParams = new object[] {decl, Datum.Parse(block, operand2)};
            var types = from t in asm.GetTypes() 
                        where 
                            (t.BaseType == typeof(ConditionStatement)) &&
                            (opToken.Equals((asm.CreateInstance(t.FullName) as IMathOperator).GetHandledOperator()))
                        select
                            Activator.CreateInstance(t, objParams) as ConditionStatement;

            if (types.Count<ConditionStatement>() > 0)
            {
                block.Statements.Add(types.First<ConditionStatement>());
            }

            return decl;
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.Append(LValue);
            sb.Append(" = ");
            sb.Append("(");
            sb.Append(CondDecl);
            sb.Append(" ");
            sb.Append(Operator);
            sb.Append(" ");
            sb.Append(CondValue);
            sb.Append(")");
            sb.Append(";");
            return sb.ToString();
        }
    }
}
