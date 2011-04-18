using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Statements.Math;
using System.Diagnostics;

namespace WabbitC.Optimizer
{
	static class CSE
	{
		public static bool Optimize(ref Module module)
		{
			bool hasChanged = false;
			var functions = module.GetFunctionEnumerator();
			while (functions.MoveNext())
			{
				Block block = functions.Current.Code;
				var basicBlocks = block.GetBasicBlocks();
				block.Statements.Clear();
				for (int i = 0; i < basicBlocks.Count; i++)
				{
					Block basicBlock = basicBlocks[i];
					hasChanged |= OptimizeBlock(ref basicBlock);
					block.Statements.AddRange(basicBlock);
				}
			}
			return hasChanged;
		}

		/*Algorithm:
		 * Source: http://www.cs.montana.edu/~bohannan/550/local_subexp_elim.html
		For each binary expression (x = z + 2, for example) in the basic block:
			If the expression, or its equivalent, has not yet been seen,
				Add the expression to our set of possible redundant expressions.
			If the expression, or its equivalent, has been seen exactly once before,
				1. Create a temporary variable to hold the computation.
				2. Add an instruction immediatly prior to the first time the instruction was seen.
				3. Replace the first and second occurances with the temporary variable.
			If the expression, or its equivalent, has been seen more than twice,
				Replace the expression with its temporary variable.
		*/
		public static bool OptimizeBlock(ref Block block)
		{
			List<CSEStore> possibleSubExp = new List<CSEStore>();
			CSEStore curExp = null;
			bool hasChanged = false;
			var statements = from Statement st in block select st;
			foreach (var statement in statements)
			{
				int index = block.Statements.IndexOf(statement);
				block.Statements.Remove(statement);
				var newStatements = new List<Statement>();

				//actual optimization code
				System.Type type = statement.GetType();
				if (type == typeof(Move))
				{
					var move = statement as Move;
					curExp = new CSEStore(index, move.LValue, move.RValue);
					move.LValue.ConstStatement = move;
				}
				else if (type.BaseType == typeof(ConditionStatement))
				{
					var cond = statement as ConditionStatement;
					curExp = new CSEStore(index, cond.CondDecl, cond.CondValue, cond.Operator);
					cond.LValue.ConstStatement = statement;
				}
				else if (type.BaseType == typeof(MathStatement))
				{
					var math = statement as MathStatement;
					if (curExp == null)
					{
						var test = math.LValue;
						if (block.FindDeclaration(math.LValue.Name) != null)
						{
							curExp = new CSEStore(index, math.LValue, math.LValue);
						}
					}
					if (curExp != null)
					{
						math.LValue.ConstStatement = null;
						curExp.Operand2 = math.RValue;
						curExp.Operator = math.Operator;
						int listIndex = possibleSubExp.IndexOf(curExp);
						if (listIndex == -1)
						{
							//do not check shit like test *= test;
							bool add = curExp.Operand2 != curExp.Operand1;
							if (possibleSubExp.Count > 0 && ((math.GetType() == typeof(Add) || math.GetType() == typeof(Sub)) && math.RValue.GetType() == typeof(Immediate)))
							{
								Immediate imm = math.RValue as Immediate;
								Immediate newImm = new Immediate(imm.Value);
								Immediate tempImm;
								for (int i = 1; i <= 4; i++)
								{
									tempImm = newImm + new Immediate(i);
									curExp.Operand2 = tempImm;
									listIndex = possibleSubExp.IndexOf(curExp);
									if (listIndex == -1)
									{
										tempImm = newImm - new Immediate(i);
										curExp.Operand2 = tempImm;
										listIndex = possibleSubExp.IndexOf(curExp);
									}
									if (listIndex != -1)
									{
										var firstInst = possibleSubExp[listIndex];
										if (firstInst.TempDecl == null)
										{
											firstInst.TempDecl = block.CreateTempDeclaration(firstInst.LValue.Type);
											block.Statements.Insert(firstInst.index + 2, new Move(firstInst.TempDecl, firstInst.LValue));
										}
										block.Statements.RemoveAt(index);
										newStatements.Add(new Move(firstInst.LValue, firstInst.TempDecl));
										for (; i > 0; i--)
										{
											if (math.Operator == "+")
												newStatements.Add(new Inc(firstInst.LValue));
											else
												newStatements.Add(new Dec(firstInst.LValue));
										}
										hasChanged = true;
										add = false;
										break;
									}

								}
							}
							if (add)
								possibleSubExp.Add(curExp);
						}
						else
						{
							var firstInst = possibleSubExp[listIndex];
							if (firstInst.TempDecl == null)
							{
								firstInst.TempDecl = block.CreateTempDeclaration(firstInst.LValue.Type);
								block.Statements.Insert(firstInst.index + 2, new Move(firstInst.TempDecl, firstInst.LValue));
							}
							block.Statements.RemoveAt(index);
							newStatements.Add(new Move(firstInst.LValue, firstInst.TempDecl));
							hasChanged = true;
						}
						curExp = null;// new CSEStore(index, math.LValue, math.RValue);
					}
				}

				if (newStatements.Count == 0)
					newStatements.Add(statement);
				block.Statements.InsertRange(index, newStatements);
			}
			return hasChanged;
		}
	}

	class CSEStore
	{
		public Declaration TempDecl = null;
		public int index;
		public Datum Operand1;
		public Datum Operand2;
		public Token Operator;
		public Declaration LValue;

		public CSEStore(int index, Declaration lValue, Datum op1)
		{
			this.index = index;
			LValue = lValue;
			Operand1 = op1;
		}

		public CSEStore(int index, Datum op1, Datum op2, Token op)
		{
			this.index = index;
			Operand1 = op1;
			Operand2 = op2;
			Operator = op;
		}

		public override bool Equals(object obj)
		{
			if (obj.GetType() != typeof(CSEStore))
				return base.Equals(obj);
			var cse = obj as CSEStore;
			return this.Operand1.Equals(cse.Operand1) && this.Operand2.Equals(cse.Operand2) && this.Operator.Equals(cse.Operator);
		}

		public override string ToString()
		{
			string temp = Operand1.ToString();
			if (Operand2 != null)
			{
				temp += Operator + Operand2;
			}
			return temp;
		}
	}
}
