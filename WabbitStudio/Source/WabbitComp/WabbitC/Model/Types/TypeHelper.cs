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
            catch (System.Exception)
            {
                // Then try User defined types (this method will have to take a Block)
            }

            return null;
        }

		public static Type GetType(Block block, Token token)
		{
			if (token.Type == TokenType.IntType)
				return new BuiltInType("int");
			if (token.Type == TokenType.RealType)
				return new BuiltInType("float");
			var decl = block.FindDeclaration(token.Text);
			if (decl == null)
				return null;
            return (Type) decl.Type.Clone();
		}
    }
}
