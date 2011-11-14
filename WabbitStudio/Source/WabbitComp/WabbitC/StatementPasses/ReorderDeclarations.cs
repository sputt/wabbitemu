using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using System.Diagnostics;

namespace WabbitC.StatementPasses
{
	/// <summary>
	/// Move all array declarations to the start
	/// </summary>
	class ReorderDeclarations
	{
		public static void Run(Module module)
		{
			//TODO: Also move structure declarations to the start
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				Block block = functions.Current.Code;
				var decls = from Declaration d in block.Declarations 
							where d.Type is Model.Types.Array
							select d;

				Declaration[] arrayDecls = decls.ToArray<Declaration>();
				foreach (Declaration array in arrayDecls)
				{
					block.Declarations.Remove(array);
					block.Declarations.Insert(0, array);
				}
			}
		}
	}
}
