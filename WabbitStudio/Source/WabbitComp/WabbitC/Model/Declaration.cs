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
            this.type = declarationType;
            this.Name = name;
        }

        public string GetDeclaration()
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

        //public ValueStatement InitialValue;
        public Block Code;
    }
}
