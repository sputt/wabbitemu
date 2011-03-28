using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model
{
    class Declaration : Datum
    {
        public Type Type;
        public String Name;

        public Declaration()
        {
        }

        public Declaration(Type declarationType, String name)
        {
            Type = declarationType;
            Name = name;
        }

        #region Allocated properties
        public int StackOffset;
        #endregion

        #region ToString methods
        public string ToDeclarationString()
        {
            if (Type == null)
            {
                return "#NOTPROVIDED# " + Name;
            }
            else
            {
                if (Type.GetType() == typeof(Types.FunctionType))
                {
                    string funcType = Type.ToString();
                    funcType = funcType.Replace("(*)", Name);
                    if (Code != null)
                    {
                        funcType += "\n" + Code.ToString();
                        return funcType;
                    }
                    else
                    {
                        return funcType + ";";
                    }
                }
                else if (Type.GetType() == typeof(Types.Array))
                {
                    var arrayType = Type as Types.Array;
                    string arrayString = Type.ToString();
                    arrayString = arrayString.Replace(arrayType.BaseType.ToString(), 
                        arrayType.BaseType.ToString() + " " + Name);
                    return arrayString + ";";
                }
                else
                {
                    return Type + " " + Name + ";";
                }
            }
        }

        public override string ToString()
        {
            return Name;
        }
        #endregion

        //public ValueStatement InitialValue;
        public Block Code;
        public Immediate ConstValue = new Immediate(Token.ZeroToken);
    }
}
