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
        };

        private BuiltInTypeType type;
        private bool isUnsigned;
        private int indirectionLevels;

        /// <summary>
        /// 
        /// </summary>
        /// <param name="tokens"></param>
        public BuiltInType(ref List<Token>.Enumerator tokens)
        {
            if (tokens.Current.TokenText == "unsigned")
            {
                isUnsigned = true;
                tokens.MoveNext();
            }
            else
            {
                isUnsigned = false;
            }

            switch (tokens.Current.TokenText.ToUpper())
            {
                case "CHAR":
                    type = BuiltInTypeType.Char;
                    this.Size = 1;
                    break;
                case "INT":
                    type = BuiltInTypeType.Int;
                    this.Size = 2;
                    break;
                default:
                    throw new System.Exception("No type for this");
            }

            indirectionLevels = 0;
            while ((tokens.MoveNext()) && (tokens.Current.TokenText == "*"))
            {
                indirectionLevels++;
            }
        }

        public bool Equals(Type otherType)
        {
            if (otherType.GetType() != typeof(BuiltInType))
            {
                return false;
            }
            BuiltInType otherBuiltInType = (BuiltInType)otherType;
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
