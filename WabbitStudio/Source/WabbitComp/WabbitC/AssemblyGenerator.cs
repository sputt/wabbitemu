using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Types;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Assembly;

namespace WabbitC
{
	static class AssemblyGenerator
	{
		public static void GenerateCode(ref Module module)
		{
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				if (functions.Current.Code != null)
				{
					Block block = functions.Current.Code;
					var statements = from Statement st in functions.Current.Code
									 select st;
					foreach (var statement in statements)
					{
						int nPos = block.Statements.IndexOf(statement);
						System.Type type = statement.GetType();
						if (type == typeof(StackLoad))
						{

						}
						else if (type == typeof(StackStore))
						{

						}
						else if (type == typeof(Goto))
						{
							var gotoStatement = statement as Goto;
							Jump jump = null;
							if (gotoStatement.CondDecl != null)
							{
								//jump = new Jump(gotoStatement.TargetLabel.Name, gotoStatement.CondDec
							}
							else
							{
								jump = new Jump(gotoStatement.TargetLabel.Name);
							}
							block.Statements.Remove(statement);
							block.Statements.Insert(nPos, jump);
						}
						else if (type == typeof(Move))
						{
							var move = statement as Move;
							var load = new Model.Statements.Assembly.Load(move.LValue, move.RValue);
							block.Statements.Remove(statement);
							block.Statements.Insert(nPos, load);
						}
					}

				}
			}
		}
	}
}
