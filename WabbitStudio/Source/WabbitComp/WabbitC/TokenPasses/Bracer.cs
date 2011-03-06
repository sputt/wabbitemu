using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.TokenPasses
{
    class Bracer : TokenPass
    {
        public override List<Token> Run(List<Token> tokenList)
        {
            return tokenList;
        }
    }
}
