using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model.Statements.Condition;

namespace WabbitC.Model.Statements
{
    class Goto : ControlStatement
    {
		public enum GotoCondition
		{
			None,
			Z,
			NZ,
			C,
			NC,

		}
        public Label TargetLabel;
		public Declaration CondDecl;
        public GotoCondition GotoCond;

        public Goto(Label lbl)
        {
            TargetLabel = lbl;
			GotoCond = Goto.GotoCondition.None;
        }

        public Goto(Label lbl, Declaration condDecl, GotoCondition cond)
        {
            TargetLabel = lbl;
			CondDecl = condDecl;
			GotoCond = cond;
        }

        public override ISet<Declaration> GetReferencedDeclarations()
        {
			var result = new HashSet<Declaration>();
			if (CondDecl != null)
			{
				result.Add(CondDecl);
			}
			return result;
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
			string gotoString = "jp ";
			switch (GotoCond)
			{
				case GotoCondition.Z:
					gotoString += "z, ";
					break;
				case GotoCondition.NZ:
					gotoString += "nz, ";
					break;
				case GotoCondition.C:
					gotoString += "c, ";
					break;
				case GotoCondition.NC:
					gotoString += "nc, ";
					break;
			}
			return gotoString + TargetLabel.Name; 
		}

		internal static List<Statement> ParseConditionStatements(List<Statement> condStatements, Label TargetLabel)
		{
			var newStatements = new List<Statement>();
			GotoCondition cond = GotoCondition.None;
			Declaration lastMoveDecl = null;
			foreach (var statement in condStatements)
			{
				var type = statement.GetType();
				if (type == typeof(Goto))
				{
					var gotoStatement = statement as Goto;
					
				}
				else if (type == typeof(Move))
				{
					var move = statement as Move;
					lastMoveDecl = move.LValue;
				}
				else if (type == typeof(Math.Not))
				{
					switch (cond)
					{
						case GotoCondition.NZ:
							cond = GotoCondition.Z;
							break;
						case GotoCondition.Z:
							cond = GotoCondition.NZ;
							break;
						case GotoCondition.C:
							cond = GotoCondition.NC;
							break;
						case GotoCondition.NC:
							cond = GotoCondition.C;
							break;
					}
				}
				else if (type == typeof(Equals))
				{
					cond = GotoCondition.Z;
				}
				else if (type == typeof(NotEquals))
				{
					cond = GotoCondition.NZ;
				}
				newStatements.Add(statement);
			}
			newStatements.Add(new Goto(TargetLabel, lastMoveDecl, cond));
			return newStatements;
		}
	}
}
