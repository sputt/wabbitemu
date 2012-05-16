using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
    class While : ControlStatement, ILoop
    {
        public Block Condition { get; set; }
        public Block Body { get; set; }
        public Label Label { get; set; }
        public Declaration CondDecl { get; set; }

        public static While Parse(Block parent, ref List<Token>.Enumerator tokens)
        {
            Debug.Assert(tokens.Current.Text == "while");

            tokens.MoveNext();
            Debug.Assert(tokens.Current.Text == "(");
            tokens.MoveNext();
            List<Token> conditionList = Tokenizer.GetArgument(ref tokens);
            tokens.MoveNext();
            Debug.Assert(tokens.Current.Text == "{");
            tokens.MoveNext();

            Block body = Block.ParseBlock(ref tokens, parent);
            tokens.MoveNext();

            var condDecl = parent.CreateTempDeclaration(new BuiltInType("int"));
            Block condBlock = new Block();
            condBlock.Parent = parent;
            condBlock.TempDeclarationNumber = parent.TempDeclarationNumber;
            StatementHelper.Parse(condBlock, condDecl, conditionList);

            return new While(parent, condDecl, condBlock, body);
        }

        public While(Block parent, Declaration decl,  Block cond, Block body)
        {
            Debug.Assert(parent != null);
            Debug.Assert(cond != null);
            Debug.Assert(body != null);

            body.Parent = parent;

            Condition = cond;
            Body = body;

            Label = parent.CreateTempLabel();

            CondDecl = decl;
        }

        public override IEnumerator<Block> GetEnumerator()
        {
            return new List<Block> { Condition, Body }.GetEnumerator();
        }

        public override string ToString()
		{
			StringBuilder sb = new StringBuilder("goto ");
			sb.Append(Label.Name);
			sb.AppendLine(";");
			sb.AppendLine("do");
			sb.AppendLine("{");

            foreach (Declaration decl in Body.Declarations.Concat(Condition.Declarations))
            {
                sb.AppendLine(decl.ToDeclarationString());
            }
            foreach (Statement statement in Body.Statements)
            {
                sb.AppendLine(statement.ToString());
            }
            sb.AppendLine(Label.ToString());
            foreach (Statement statement in Condition.Statements)
            {
                sb.AppendLine(statement.ToString());
            }
            sb.AppendLine("}");
            sb.Append("while (");
			sb.Append(CondDecl.Name);
			sb.Append(");");
            
            return sb.ToString();
        }
    }
}
