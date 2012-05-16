using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Diagnostics;

using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
    class FunctionCall : ValueStatement
    {
        public Declaration LValue;
        public Declaration Function;
        public List<Declaration> Params;

        public static List<Declaration> BuildParams(Block block, FunctionType funcType, ref List<Token>.Enumerator tokens)
        {
            var tokenList = new List<Declaration>();
            int nCount = 0;
            while (tokens.Current.Type != TokenType.CloseParen)
            {
                Declaration decl = block.CreateTempDeclaration(funcType.Params[nCount].Type);

                var valueList = Tokenizer.GetArgument(ref tokens);

                StatementHelper.Parse(block, decl, valueList);

                tokenList.Add(decl);
  
                Debug.Assert(tokens.Current.Text == "," || tokens.Current.Text == ")");

                if (tokens.Current.Text == ",")
                    tokens.MoveNext();
            }
            return tokenList;
        }

		public static List<Declaration> BuildParams(Block block, FunctionType funcType, List<Expression> args)
		{
            if (funcType.Params.Count == 0)
            {
                return new List<Declaration>();
            }
			var tokenList = new List<Declaration>();
			int nCount = 0;
			foreach(Expression arg in args)
			{
				Declaration decl = block.CreateTempDeclaration(funcType.Params[nCount].Type);

                StatementHelper.Parse(block, decl, arg.Tokens);

				tokenList.Add(decl);
			}
			return tokenList;
		}

        public FunctionCall(Declaration returnDecl, Declaration func, List<Declaration> funcParams)
        {
            Debug.Assert(func.Type is FunctionType);
            LValue = returnDecl;
            Function = func;
            Params = funcParams;
        }

		public override ISet<Declaration> GetModifiedDeclarations()
        {
            if (LValue != null)
            {
                return new HashSet<Declaration>() { LValue };
            }
            return new HashSet<Declaration>() { };
        }

        public override ISet<Declaration> GetReferencedDeclarations()
        {
			var temp = new HashSet<Declaration>();
			temp.Add(Function);
			temp.UnionWith(Params);
			return temp;
        }

        public override string ToString()
        {
            string result = String.Empty;
            if (LValue != null)
            {
                result += LValue.Name + " = ";
            }
            result += Function.Name;
            result += "(";

            for (int i = 0; i < Params.Count; i++)
            {
                result += Params[i].Name;
                if (i != Params.Count - 1)
                    result += ", ";
            }
            result += ");";
            return result;
        }
    }
}
