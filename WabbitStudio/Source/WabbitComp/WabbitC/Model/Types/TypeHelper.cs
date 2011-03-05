using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Types
{
    static class TypeHelper
    {
        static public Type ParseType(ref List<Token>.Enumerator tokens)
        {
            try
            {
                BuiltInType builtIn = new BuiltInType(ref tokens);
                return builtIn;
            }
            catch (System.Exception e)
            {
                // Then try User defined types (this method will have to take a Block)
            }

            return null;
        }
    }
}
