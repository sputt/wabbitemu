using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using WabbitC.Model.Types;

namespace WabbitC.Model
{
    class Module
    {
        private List<Declaration> declarations;

        public Module(List<Token> tokenList)
        {
            List<Token>.Enumerator tokens = tokenList.GetEnumerator();
            tokens.MoveNext();

            declarations = new List<Declaration>();

            while (tokens.Current != null)
            {
                if (tokens.Current.TokenText == "typedef")
                {

                }
                else if (tokens.Current.TokenText == "struct")
                {
                    tokens.MoveNext();
                    if (tokens.Current.TokenType != TokenType.OpenBlock)
                    {
                        throw new System.Exception("That was gay");
                    }
                }

                Type resultType = null;
                String resultName = "";

                Type first = TypeHelper.ParseType(ref tokens);
                if (first == null)
                {
                    throw new System.Exception("Could not parse a type");
                }

                // Read the name of the declaration/type
                if (tokens.Current.TokenType == TokenType.StringType)
                {
                    resultName = tokens.Current.TokenText;
                    tokens.MoveNext();
                }

                // in this case it's either a prototype or a function
                // Either way it gets added to the types of this module
                if (tokens.Current.TokenType == TokenType.OpenParen)
                {
                    FunctionType function = new FunctionType(ref tokens, first);
                    resultType = function;
                }
                else
                {
                    resultType = first;
                }

                Debug.Assert(tokens.Current.TokenType == TokenType.StatementEnd);
                tokens.MoveNext();

                declarations.Add(new Declaration(resultType, resultName));
            }

            // Got all the declarations now
            Debug.WriteLine("Done");
        }
    }
}
