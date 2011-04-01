using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model.Types
{
	class StrippedFunctionType : FunctionType
	{
		public StrippedFunctionType(FunctionType copy)
		{
			ReturnType = copy.ReturnType.Clone() as Type;

			Params = new List<Declaration>();
			foreach (Declaration param in copy.Params)
			{
				Params.Add(param);
			}
		}

		public override string ToDeclarationString(string DeclName)
		{
			return "void " + DeclName + "()";
		}
	}
}
