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
        Declaration LValue;
        Declaration Function;
        List<Declaration> Params;

        public static List<Declaration> BuildParams(Block block, FunctionType funcType, ref List<Token>.Enumerator tokens)
        {
            var tokenList = new List<Declaration>();
            int nCount = 0;
            while (tokens.Current.Type != TokenType.CloseParen)
            {
                Declaration decl = block.CreateTempDeclaration(funcType.ParamDefs[nCount].Type);

                var valueList = Tokenizer.GetArgument(ref tokens);
                var statement = new Assignment(decl, new Immediate(valueList));
                block.Statements.Add(statement);
                tokenList.Add(decl);
  
                Debug.Assert(tokens.Current.Text == "," || tokens.Current.Text == ")");

                if (tokens.Current.Text == ",")
                    tokens.MoveNext();
            }
            return tokenList;
        }

        public FunctionCall(Declaration returnDecl, Declaration func, List<Declaration> funcParams)
        {
            Debug.Assert(func.Type.GetType() == typeof(FunctionType));
            LValue = returnDecl;
            Function = func;
            Params = funcParams;
        }

        public override string ToString()
        {
            string result = "";
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
