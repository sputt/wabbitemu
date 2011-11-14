using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model.Statements;

namespace WabbitC.Model
{
    class Declaration : Datum, ICloneable
    {
        public Type Type;
        public String Name;

        public Declaration()
        {
            Properties = new HashSet<string>();
        }

        public Declaration(Type declarationType, String name) : this()
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
				if (Type is Types.FunctionType)
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

        public override bool Equals(object obj)
        {
            if (!(obj is Declaration))
                return base.Equals(obj);
            var decl = (Declaration)obj;
            return decl.Name == this.Name && this.Type.Equals(decl.Type);
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
        public HashSet<String> Properties;

        public object Clone()
        {
            return this.MemberwiseClone();
        }
    }
}
