using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Label : ControlStatement
    {
        public string Name;

        public Label(string name)
        {
            Name = name;
        }

        public override string ToString()
        {
            return Name + ":\n;";
        }
    }
}
