using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model
{
    class Declaration : Datum
    {
        private Type type;
        public Type Type
        {
            get
            {
                return type;
            }
        }

        public String Name;

        public Declaration(Type declarationType, String name)
        {
            type = declarationType;
            Name = name;
        }

        #region ToString methods
        public string ToDeclarationString()
        {
            if (type == null)
            {
                return "#NOTPROVIDED# " + Name;
            }
            else
            {
                if (type.GetType() == typeof(Types.FunctionType))
                {
                    string funcType = type.ToString();
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
                else if (type.GetType() == typeof(Types.Array))
                {
                    var arrayType = type as Types.Array;
                    string arrayString = type.ToString();
                    arrayString = arrayString.Replace(arrayType.BaseType.ToString(), 
                        arrayType.BaseType.ToString() + " " + Name);
                    return arrayString + ";";
                }
                else
                {
                    return type + " " + Name + ";";
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
        public Immediate ConstValue;
    }
}
