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

		public BuiltInTypeType BuildInTypeType;
		private bool isUnsigned;
		public BuiltInType()
		{
			BuildInTypeType = BuiltInTypeType.Int;
			isUnsigned = false;
			IndirectionLevels = 0;
			this.Size = 4;
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
					BuildInTypeType = BuiltInTypeType.Char;
					this.Size = 1;
					break;
				case "int":
					BuildInTypeType = BuiltInTypeType.Int;
					this.Size = 4;
					break;
				case "void":
					BuildInTypeType = BuiltInTypeType.Void;
					this.Size = 0;
					break;
				default:
					throw new System.Exception("No type for this");
			}

			IndirectionLevels = 0;
			while ((tokens.MoveNext()) && (tokens.Current.Text == "*"))
			{
				IndirectionLevels++;
				this.Size = 4;
			}
		}

		public BuiltInType(string typeString)
		{
			var tokens = Tokenizer.Tokenize(typeString).GetEnumerator();
			tokens.MoveNext();
			InitializeType(ref tokens);
		}

		public BuiltInType(ref List<Token>.Enumerator tokens)
		{
			InitializeType(ref tokens);
		}

		public override object Clone()
		{
			return new BuiltInType(this.ToString());
		}

		public override string ToString()
		{
			StringBuilder sb = new StringBuilder();
			if (isUnsigned)
				sb.Append("unsigned ");
			sb.Append(BuildInTypeType.ToString().ToLower());
			for (int i = 0; i < IndirectionLevels; i++)
				sb.Append("*");
			
			return sb.ToString();
		}

		public override string ToDeclarationString(string DeclName)
		{
			return this.ToString() + " " + DeclName + ";";
		}

		public override bool Equals(object obj)
		{
			if (!(obj is BuiltInType))
			{
				return false;
			}
			BuiltInType otherBuiltInType = (BuiltInType)obj;
			if (otherBuiltInType.BuildInTypeType != this.BuildInTypeType)
			{
				return false;
			}
			if (otherBuiltInType.isUnsigned != this.isUnsigned)
			{
				return false;
			}
			return true;
		}

        public override int GetHashCode()
        {
            return this.BuildInTypeType.GetHashCode() + this.isUnsigned.GetHashCode() + this.IndirectionLevels.GetHashCode();
        }
	}
}
