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
		public Block ConditionBlock;
        public Block TrueCase;
        public Block FalseCase;

        public static If ParseIf(Block parent, ref List<Token>.Enumerator tokens)
        {
            Debug.Assert(tokens.Current.Text == "if");

            tokens.MoveNext();
            Debug.Assert(tokens.Current.Text == "(");
            tokens.MoveNext();

            List<Token> conditionList = Tokenizer.GetArgument(ref tokens);

			var condBlock = new Block(parent);
            Declaration condDecl = parent.CreateTempDeclaration(new BuiltInType("int"));

            StatementHelper.Parse(condBlock, condDecl, conditionList);			

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

            return new If(parent, condDecl, condBlock, trueCase, falseCase);
        }

        public If(Block parent, Declaration cond, Block condBlock, Block trueCase, Block falseCase)
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
			ConditionBlock = condBlock;
        }

        public override string ToString()
        {
			StringBuilder result = new StringBuilder();
			foreach (var statement in ConditionBlock.Statements)
			{
				result.AppendLine(statement.ToString());
			}
			result.Append("if (");
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

        #region IEnumerable<Block> Members

        public override IEnumerator<Block> GetEnumerator()
        {
            List<Block> blocks = new List<Block>();
			blocks.Add(ConditionBlock);
			blocks.Add(TrueCase);
            if (FalseCase != null)
            {
                blocks.Add(FalseCase);
            }
            return blocks.GetEnumerator();
        }

        #endregion
    }
}
