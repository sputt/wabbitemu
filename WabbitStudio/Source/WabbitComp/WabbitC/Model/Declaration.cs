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

        //public ValueStatement InitialValue;
        public Block Code;
    }
}
