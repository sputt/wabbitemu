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

        /// <summary>
        /// 
        /// </summary>
        /// <param name="tokens"></param>
        public BuiltInType(ref List<Token>.Enumerator tokens)
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

        public override string ToString()
        {
            return type.ToString().ToLower();
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
