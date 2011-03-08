using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Diagnostics;

namespace WabbitC.Model.Types
{
    public class FunctionType : Type
    {
        public Type ReturnType;
        public struct ParamDef
        {
            public Type Type;
            public String Name; 
        }
        public List<ParamDef> ParamDefs;

        public FunctionType(ref List<Token>.Enumerator tokens, Type returnType)
        {
            this.Size = 2;

            Debug.Assert(tokens.Current.Type == TokenType.OpenParen);
            
            tokens.MoveNext();

            this.ReturnType = returnType;

            ParamDefs = new List<ParamDef>();
            while (tokens.Current.Type != TokenType.CloseParen)
            {
                ParamDef param;
                param.Type = TypeHelper.ParseType(ref tokens);
                param.Name = tokens.Current.Text;
                tokens.MoveNext();

                ParamDefs.Add(param);

                Debug.Assert(tokens.Current.Text == "," || tokens.Current.Text == ")");

                if (tokens.Current.Text == ",")
                    tokens.MoveNext();
            }

            tokens.MoveNext();
        }

        public override string ToString()
        {
            string result = ReturnType + " (*)(";
            for (int i = 0; i < ParamDefs.Count; i++)
            {
                result += ParamDefs[i].Type + " " + ParamDefs[i].Name;
                if (i != ParamDefs.Count - 1)
                    result += ", ";
            }
            result += ")";
            return result;
        }
    }
}
