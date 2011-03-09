using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
    class While : ControlStatement
    {
		public Declaration Condition;
        public Block LoopCase;

        public static While ParseWhile(Block parent, ref List<Token>.Enumerator tokens)
        {
            Debug.Assert(tokens.Current.Text == "while");

            tokens.MoveNext();
            Debug.Assert(tokens.Current.Text == "(");
            tokens.MoveNext();

            List<Token> conditionList = Tokenizer.GetArgument(ref tokens);

            Declaration condDecl = parent.CreateTempDeclaration(new BuiltInType("int"));

            AssignmentHelper.Parse(parent, condDecl, conditionList);

            tokens.MoveNext();
            Debug.Assert(tokens.Current.Text == "{");
            tokens.MoveNext();

            Block loopCase = Block.ParseBlock(ref tokens, parent);
            tokens.MoveNext();

            return new While(parent, condDecl, loopCase);
        }

        public While(Block parent, Declaration cond, Block loopCase)
        {
            Debug.Assert(loopCase != null);

            loopCase.Parent = parent;

            Condition = cond;
			LoopCase = loopCase;
        }

        public override string ToString()
        {
			StringBuilder result = new StringBuilder("while (");
			result.Append(Condition.Name);
			result.AppendLine(")");
            result.Append(LoopCase);
            
            return result.ToString();
        }
    }
}
