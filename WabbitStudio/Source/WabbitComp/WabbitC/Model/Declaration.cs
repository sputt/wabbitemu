using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model
{
    class Declaration
    {
        private Type type;
        public Type Type
        {
            get
            {
                return type;
            }
        }

        private String name;
        public String Name { get { return name; } }

        public Declaration(Type declarationType, String name)
        {
            this.type = declarationType;
            this.name = name;
        }

        public override string ToString()
        {
            if (type == null)
            {
                return "#NOTPROVIDED# " + name;
            }
            else
            {
                if (type.GetType() == typeof(Types.FunctionType))
                {
                    string funcType = type.ToString();
                    funcType = funcType.Replace("(*)", name);
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
                    return type + " " + name + ";";
                }
            }
        }

        //public ValueStatement InitialValue;
        public Block Code;
    }
}
