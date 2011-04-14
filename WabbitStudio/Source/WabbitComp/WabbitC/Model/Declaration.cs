using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model.Statements;

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

        #region ToString methods
        public string ToDeclarationString()
        {
            if (Type == null)
            {
                return "#NOTPROVIDED# " + Name;
            }
            else
            {
				if (Type.GetType() == typeof(Types.FunctionType) || Type.GetType().BaseType == typeof(Types.FunctionType))
                {
                    string funcType = Type.ToDeclarationString(Name);
                    if (Code != null)
                    {
						funcType += Environment.NewLine + Code.ToString();
                        return funcType;
                    }
                    else
                    {
                        return funcType + ";";
                    }
                }
                else
                {
                    return Type.ToDeclarationString(Name);
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
		public Statement ConstStatement;
        public bool IsAlive;
    }
}
