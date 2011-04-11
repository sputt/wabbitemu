using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Types;

namespace WabbitC.Model.Statements
{
	class Call : ControlStatement
	{
		public Declaration CallTarget;
		public Declaration ReturnRegister
		{
			get
			{
				FunctionType type = CallTarget.Type as FunctionType;
				return (type.Is16() ? Block.FindDeclaration("__hl") : Block.FindDeclaration("__a"));
			}
		}

		public Call(Declaration callTarget)
		{
			CallTarget = callTarget;
			
			FunctionType type = callTarget.Type as FunctionType;
			if (type == null)
			{
				throw new System.Exception("Call: Not a function");
			}
		}

		public override ISet<Declaration> GetReferencedDeclarations()
		{
			return new HashSet<Declaration>() { };
		}

		public override ISet<Declaration> GetModifiedDeclarations()
		{
			return new HashSet<Declaration>() { ReturnRegister };
		}

		public override string ToString()
		{
			return CallTarget.Name + "();";
		}

		public override string ToAssemblyString()
		{
			return "call " + CallTarget.Name;
		}
	}
}
