using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Diagnostics;

namespace WabbitC.Model.Types
{
    class FunctionType : Type
    {
        Type returnType;
        struct ParamDef
        {
            public Type Type;
            public String Name; 
        }
        List<ParamDef> paramDefs;

        public FunctionType(ref List<Token>.Enumerator tokens, Type returnType)
        {
            this.Size = 2;

            Debug.Assert(tokens.Current.TokenType == TokenType.OpenParen);
            
            tokens.MoveNext();

            this.returnType = returnType;

            paramDefs = new List<ParamDef>();
            while (tokens.Current.TokenType != TokenType.CloseParen)
            {
                ParamDef param;
                param.Type = TypeHelper.ParseType(ref tokens);
                param.Name = tokens.Current.TokenText;
                tokens.MoveNext();

                paramDefs.Add(param);

                Debug.Assert(tokens.Current.TokenText == "," || tokens.Current.TokenText == ")");

                if (tokens.Current.TokenText == ",")
                    tokens.MoveNext();
            }

            tokens.MoveNext();
        }
    }
}
