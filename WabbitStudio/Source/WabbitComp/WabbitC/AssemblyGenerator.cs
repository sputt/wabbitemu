using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Types;

namespace WabbitC
{
	class AssemblyGenerator
	{
		Block block;
		public AssemblyGenerator(Block block)
		{
			this.block = block;
		}

		AssemblyVars varNames = new AssemblyVars();
		public string GenerateCode()
		{
			StringBuilder sb = new StringBuilder();
			foreach (Declaration decl in block.Declarations)
			{
				if (decl.Type.GetType() == typeof(FunctionType))
				{
					AssemblyGenerator generator = new AssemblyGenerator(decl.Code);
					string code = generator.GenerateCode();
					sb.Append(code);
				}
				else if (decl.Type.GetType().BaseType == typeof(WabbitC.Model.Type))
				{
					varNames.AddDeclaration(decl);
				}
			}
			string variables = varNames.ToString();
			return sb.ToString();
		}
	}

	class AssemblyVars
	{
		List<Declaration> decls = new List<Declaration>();
		public AssemblyVars()
		{
		}

		public void AddDeclaration(Declaration decl)
		{
			decls.Add(decl);
		}

		public override string ToString()
		{
			string varStart = "VariableAllocationStart";
			StringBuilder sb = new StringBuilder();
			foreach (Declaration decl in decls)
			{
				sb.Append(decl.Name);
				sb.Append(" = ");
				sb.AppendLine(varStart);
				StringBuilder varNameBuilder = new StringBuilder(decl.Name);
				varNameBuilder.Append(" + ");
				varNameBuilder.Append(decl.Type.Size);
				varStart = varNameBuilder.ToString();
			}
			return sb.ToString();
		}
	}
}
