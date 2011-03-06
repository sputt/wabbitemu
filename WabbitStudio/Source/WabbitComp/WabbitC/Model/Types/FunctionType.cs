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

            Debug.Assert(tokens.Current.Type == TokenType.OpenParen);
            
            tokens.MoveNext();

            this.returnType = returnType;

            paramDefs = new List<ParamDef>();
            while (tokens.Current.Type != TokenType.CloseParen)
            {
                ParamDef param;
                param.Type = TypeHelper.ParseType(ref tokens);
                param.Name = tokens.Current.Text;
                tokens.MoveNext();

                paramDefs.Add(param);

                Debug.Assert(tokens.Current.Text == "," || tokens.Current.Text == ")");

                if (tokens.Current.Text == ",")
                    tokens.MoveNext();
            }

            tokens.MoveNext();
        }
    }
}
