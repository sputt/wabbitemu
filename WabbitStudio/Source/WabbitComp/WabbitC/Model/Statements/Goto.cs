using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Statements
{
    class Goto : ControlStatement
    {
        public Label TargetLabel;
        public Declaration CondDecl;

        public Goto(Label lbl)
        {
            TargetLabel = lbl;
            CondDecl = null;
        }

        public Goto(Label lbl, Declaration condDecl)
        {
            TargetLabel = lbl;
            CondDecl = condDecl;
        }

        public override List<Declaration> GetReferencedDeclarations()
        {
            if (CondDecl != null)
            {
                return new List<Declaration>() { CondDecl };
            }
            else
            {
                return base.GetReferencedDeclarations();
            }
        }

        public override string ToString()
        {
			string gotoString = "goto " + TargetLabel.Name + ";";
            if (CondDecl != null)
            {
                return "if (" + CondDecl + ") {" + gotoString + "}";
            }
            else
            {
                return gotoString;
            }
        }

		public override string ToAssemblyString()
		{
			string gotoString = "jp " + TargetLabel.Name;
			if (CondDecl != null)
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
