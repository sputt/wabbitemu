using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Diagnostics;
using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
    class If : ControlStatement
    {
        public Declaration Condition;
        public Block TrueCase;
        public Block FalseCase;

        public static If ParseIf(Block parent, ref List<Token>.Enumerator tokens)
        {
            Debug.Assert(tokens.Current.Text == "if");

            tokens.MoveNext();
            Debug.Assert(tokens.Current.Text == "(");
            tokens.MoveNext();

            List<Token> conditionList = Tokenizer.GetArgument(ref tokens);

            Declaration condDecl = parent.CreateTempDeclaration(new BuiltInType("int"));

            StatementHelper.Parse(parent, condDecl, conditionList);

            tokens.MoveNext();
            Debug.Assert(tokens.Current.Text == "{");
            tokens.MoveNext();

            Block trueCase = Block.ParseBlock(ref tokens, parent);
            tokens.MoveNext();

            Block falseCase = null;
            if (tokens.Current.Text == "else")
            {
                tokens.MoveNext();
                Debug.Assert(tokens.Current.Text == "{");
                tokens.MoveNext();

                falseCase = Block.ParseBlock(ref tokens, parent);
                tokens.MoveNext();
            }

            return new If(parent, condDecl, trueCase, falseCase);
        }

        public If(Block parent, Declaration cond, Block trueCase, Block falseCase)
        {
            Debug.Assert(trueCase != null);

            trueCase.Parent = parent;
            if (falseCase != null)
            {
                falseCase.Parent = parent;
            }

            Condition = cond;
            TrueCase = trueCase;
            FalseCase = falseCase;
        }

        public override string ToString()
        {
			StringBuilder result = new StringBuilder("if (");
			result.Append(Condition.Name);
			result.AppendLine(")");
            result.Append(TrueCase);
            if (FalseCase != null)
            {
                result.AppendLine("else");
                result.Append(FalseCase);
            }
            return result.ToString();
        }
    }
}
