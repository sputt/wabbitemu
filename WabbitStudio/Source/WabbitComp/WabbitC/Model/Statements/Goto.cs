using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Goto : ControlStatement
    {
        Label TargetLabel;
        Declaration CondDecl;

        Goto(Label lbl)
        {
            TargetLabel = lbl;
            CondDecl = null;
        }

        Goto(Label lbl, Declaration condDecl)
        {
            TargetLabel = lbl;
            CondDecl = condDecl;
        }

        public override string ToString()
        {
            string gotoString = "goto " + TargetLabel + ";";
            if (CondDecl == null)
            {
                return "if (" + CondDecl + ") {" + gotoString + "}";
            }
            else
            {
                return gotoString;
            }
        }
    }
}
