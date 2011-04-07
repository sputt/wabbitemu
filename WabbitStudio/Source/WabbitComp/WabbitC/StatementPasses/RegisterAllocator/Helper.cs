using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;

namespace WabbitC.StatementPasses.RegisterAllocator
{
	class Helper
	{
		private class Register
		{
			public string Name;
			public List<Register> Components;
			public Register Parent;
			public WabbitC.Model.Type Type;
			public Declaration Decl;

			public Declaration AssignedDecl;

			public Register(string name)
			{
				Name = name;
			}

			public void Assign(Declaration decl)
			{
				AssignedDecl = decl;
			}

			public override string ToString()
			{
				return Name;
			}
		}

		Block Block;
		List<Register> Registers;

		public Helper(Block block)
		{
			string[,] registers = new string[,]
			{
				{"__hl", "__h", "__l"},
				{"__de", "__d", "__e"},
				{"__bc", "__b", "__c"},
			};

			Registers = new List<Register>();

			for (int i = 0; i <= registers.GetUpperBound(0); i++)
			{
				var Pair = new Register(registers[i, 0]);
				Pair.Type = new WabbitC.Model.Types.BuiltInType("int");
				var R1 = new Register(registers[i, 1]);
				R1.Parent = Pair;
				R1.Type = new WabbitC.Model.Types.BuiltInType("char");

				var R2 = new Register(registers[i, 2]);
				R2.Parent = Pair;
				R2.Type = new WabbitC.Model.Types.BuiltInType("char");

				Pair.Components = new List<Register>() {R1, R2};
				Registers.Add(Pair);
				Registers.Add(R1);
				Registers.Add(R2);
			}

			var A = new Register("__a");
			A.Type = new WabbitC.Model.Types.BuiltInType("char");
			Registers.Add(A);
			Block = block;	
		}

		private void UpdateUseCount(ref Dictionary<Declaration, int> dict, Declaration decl)
		{
			if (!dict.ContainsKey(decl))
			{
				dict[decl] = 1;
			}
			else
			{
				dict[decl]++;
			}
		}


		private bool IsRegisterFree(Register reg)
		{
			if (reg.AssignedDecl != null)
			{
				return false;
			}
			if (reg.Components != null)
			{
				return IsRegisterFree(reg.Components[0]) && IsRegisterFree(reg.Components[1]);
			}
			return true;
		}

		private Register Alloc8(Declaration decl)
		{
			return null;
		}


		private Register Merge8s(Register r1, Register r2, ref List<Statement> addstatements)
		{
			Register r1_8 = (r1.Components[0].AssignedDecl == null) ? r1.Components[0] : r1.Components[1];
			Register r2_8 = (r2.Components[0].AssignedDecl != null) ? r2.Components[0] : r2.Components[1];

			addstatements.Add(new Move(r1_8.Decl, r2_8.Decl));
			r1_8.Assign(r2_8.AssignedDecl);
			r2_8.AssignedDecl = null;

			return r1;
		}

		private Register Alloc16(Declaration decl, ref List<Statement> addstatements)
		{
			// List of registers with only 1 component assigned
			var PartialAssignList = new List<Register>();
			foreach (var reg in Registers)
			{
				if (reg.Type.Size == 2)
				{
					if (IsRegisterFree(reg))
					{
						reg.Assign(decl);
						return reg;
					}
					else
					{
						if (reg.Components[0].AssignedDecl == null ||
							reg.Components[1].AssignedDecl == null)
						{
							PartialAssignList.Add(reg);
						}
					}
				}
			}

			if (PartialAssignList.Count >= 2)
			{
				var reg = Merge8s(PartialAssignList[0], PartialAssignList[1], ref addstatements);
				if (reg != null)
				{
					return reg;
				}
			}

			return null;
		}


		Register IsAllocated(Declaration decl)
		{
			foreach (var reg in Registers)
			{
				if (reg.AssignedDecl == decl)
				{
					return reg;
				}
				if (reg.Components != null)
				{
					if (reg.Components[0].AssignedDecl == decl)
					{
						return reg.Components[0];
					}
					if (reg.Components[1].AssignedDecl == decl)
					{
						return reg.Components[1];
					}
				}
			}
			return null;
		}


		Declaration AllocateRegister(Declaration decl, ref List<Statement> addstatements)
		{
			var reg = IsAllocated(decl);
			if (reg != null)
			{
				return reg.Decl;
			}

			bool Is16 = false;
			if (decl.Type.IndirectionLevels > 0)
			{
				Is16 = true;
			}
			else if (decl.Type.Size > 1)
			{
				Is16 = true;
			}

			if (Is16)
			{
				reg = Alloc16(decl, ref addstatements);
			}
			else
			{
				reg = Alloc8(decl);
			}

			if (reg != null)
			{
				return reg.Decl;
			}
			else
			{
				return null;
			}
		}

		public List<Declaration> GetMostReferencedVariables()
		{
			Dictionary<Declaration, int> RefCounts = new Dictionary<Declaration, int>();
			foreach (Statement st in Block.Statements)
			{
				foreach (Declaration decl in st.GetReferencedDeclarations())
				{
					UpdateUseCount(ref RefCounts, decl);
				}
			}

			var items = from k in RefCounts.Keys
						orderby RefCounts[k] descending
						select k;
			return items.ToList<Declaration>();
		}

		public List<Declaration> GetMostModifiedVariables()
		{
			Dictionary<Declaration, int> RefCounts = new Dictionary<Declaration, int>();
			foreach (Statement st in Block.Statements)
			{
				foreach (Declaration decl in st.GetModifiedDeclarations())
				{
					UpdateUseCount(ref RefCounts, decl);
				}
			}

			var items = from k in RefCounts.Keys
						orderby RefCounts[k] descending
						select k;
			return items.ToList<Declaration>();
		}

		public List<Statement> GetVariableStatements(Declaration decl)
		{
			var statements = from s in Block.Statements
							 where s.GetReferencedDeclarations().Contains(decl)
							 select s;
			return statements.ToList<Statement>();
		}
	}
}
