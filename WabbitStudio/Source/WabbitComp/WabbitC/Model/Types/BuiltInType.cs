using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Types
{
    class BuiltInType : Type
    {
        public enum BuiltInTypeType
        {
            Char,
            Short,
            Int,
            Long,
            Float,
            Double,
            Void,
        };

        private BuiltInTypeType type;
        private bool isUnsigned;
        private int indirectionLevels;

        public BuiltInType()
        {
            type = BuiltInTypeType.Int;
            isUnsigned = false;
            indirectionLevels = 0;
        }

        private void InitializeType(ref List<Token>.Enumerator tokens)
        {
            if (tokens.Current.Text == "unsigned")
            {
                isUnsigned = true;
                tokens.MoveNext();
            }
            else
            {
                isUnsigned = false;
            }

            switch (tokens.Current.Text)
            {
                case "char":
                    type = BuiltInTypeType.Char;
                    this.Size = 1;
                    break;
                case "int":
                    type = BuiltInTypeType.Int;
                    this.Size = 2;
                    break;
                case "void":
                    type = BuiltInTypeType.Void;
                    this.Size = 0;
                    break;
                default:
                    throw new System.Exception("No type for this");
            }

            indirectionLevels = 0;
            while ((tokens.MoveNext()) && (tokens.Current.Text == "*"))
            {
                indirectionLevels++;
                this.Size = 2;
            }
        }

        public BuiltInType(string typeString)
        {
            Tokenizer tokenizer = new Tokenizer();
            tokenizer.Tokenize(typeString);

            var tokens = tokenizer.Tokens.GetEnumerator();
            tokens.MoveNext();
            InitializeType(ref tokens);
        }

        public BuiltInType(ref List<Token>.Enumerator tokens)
        {
            InitializeType(ref tokens);
        }

        public override string ToString()
        {
			StringBuilder sb = new StringBuilder();
			if (isUnsigned)
				sb.Append("unsigned ");
			sb.Append(type.ToString().ToLower());
			for (int i = 0; i < indirectionLevels; i++)
				sb.Append("*");
			
			return sb.ToString();
        }

        public override bool Equals(object obj)
        {
            if (obj.GetType() != typeof(BuiltInType))
            {
                return false;
            }
            BuiltInType otherBuiltInType = (BuiltInType)obj;
            if (otherBuiltInType.type != this.type)
            {
                return false;
            }
            if (otherBuiltInType.isUnsigned != this.isUnsigned)
            {
                return false;
            }
            return true;
        }
    }
}
