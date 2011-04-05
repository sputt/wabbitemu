using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Types;

namespace WabbitC.Model
{
    class Immediate : Datum
    {
        public BuiltInType Type;
        public Token Value;

        public static bool IsImmediate(Token immToken)
        {
            return immToken.Type == TokenType.IntType || immToken.Type == TokenType.RealType;
            /*int number1;
            double number2;

            return int.TryParse(immToken.Text, out number1) || double.TryParse(immToken.Text, out number2);*/
        }

        public Immediate(Token valueToken)
        {
            Type = new BuiltInType();
            Value = valueToken;
        }

		public Immediate(int val)
		{
			var expr = new Expression(Tokenizer.Tokenize(val.ToString())).Eval();
			Type = new BuiltInType("int");
			Value = expr[0].Tokens[0];
		}

		public void Negate()
		{
			string negString = "0 - " + Value;
			List<Token> tokens = Tokenizer.Tokenize(negString);
			Expression expr = new Expression(tokens);
			var result = expr.Eval();
			Value = result[0].Tokens[0];
		}

        public override string ToString()
        {
            return Value.ToString();
        }
    }
}
