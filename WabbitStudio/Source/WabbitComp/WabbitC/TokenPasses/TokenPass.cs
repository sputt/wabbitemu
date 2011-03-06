using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.TokenPasses
{
    abstract class TokenPass
    {
        abstract public List<Token> Run(List<Token> tokenList);
    }
}
