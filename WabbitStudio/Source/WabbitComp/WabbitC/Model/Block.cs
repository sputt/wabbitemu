using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using WabbitC.Model.Types;
using WabbitC.Model.Statements;

namespace WabbitC.Model
{
    class BlockStatements : List<Statement>
    {
        private Block Block;

        public BlockStatements(Block block)
        {
            Block = block;
        }

        public void Add(Statement item)
        {
            item.Block = Block;
            base.Add(item);
        }

        public void AddRange(IEnumerable<Statement> collection)
        {
            foreach (Statement st in collection)
            {
                st.Block = Block;
            }
            base.AddRange(collection);
        }

        public void Insert(int index, Statement item)
        {
            item.Block = Block;
            base.Insert(index, item);
        }

        public void InsertRange(int index, IEnumerable<Statement> collection)
        {
            foreach (Statement st in collection)
            {
                st.Block = Block;
            }
            base.InsertRange(index, collection);
        }
    }

    class Block : IEnumerable<Block>, IEnumerable<Statement>
    {
        public Block Parent;
        public FunctionType Function;
        public HashSet<Type> Types;
        public List<Declaration> Declarations;
        public BlockStatements Statements;
		public StackAllocator stack;
        public int TempDeclarationNumber = 0;

		public Module Module
		{
			get
			{
				Block curBlock = this;
				while (curBlock != null && curBlock.GetType() != typeof(Module))
				{
					curBlock = curBlock.Parent;
				}
				return curBlock as Module;
			}
		}

        public Declaration FindDeclaration(String name)
        {
            Block curBlock = this;
            while (curBlock != null)
            {
                if (curBlock.Declarations != null)
                {
                    foreach (Declaration decl in curBlock.Declarations)
                    {
                        if (decl.Name == name)
                        {
                            return decl;
                        }
                    }
                }
                if (curBlock.Function != null)
                {
                    foreach (Declaration decl in curBlock.Function.Params)
                    {
                        if (decl.Name == name)
                        {
                            return decl;
                        }
                    }
                }
                curBlock = curBlock.Parent;
            }
            return null;
        }

        private static void IncrementTempDeclarationNumber(Block block)
        {
            while (block != null)
            {
                block.TempDeclarationNumber++;
                block = block.Parent;
            }
        }

        public Declaration CreateTempDeclaration(Type type)
        {
            if (type.GetType() == typeof(Types.Array))
            {
                type = (type as Types.Array).GetArrayPointerType();
            }
            var declaration = new Declaration(type, "__temp" + TempDeclarationNumber);
            Declarations.Add(declaration);

            IncrementTempDeclarationNumber(this);
            
			return declaration;
        }

        public Label CreateTempLabel()
        {
            var label = new Label("__label" + TempDeclarationNumber);
            IncrementTempDeclarationNumber(this);
            return label;
        }

		public List<Block> GetBasicBlocks()
		{
			var blocks = new List<Block>();
			Block currentBlock = new Block(this);
			currentBlock.Declarations = this.Declarations;
			currentBlock.Function = this.Function;
			currentBlock.stack = stack;
			for (int i = 0; i < Statements.Count; i++)
			{
				var statement = Statements[i];
				if (statement.GetType() == typeof(Goto) || statement.GetType() == typeof(Label))
				{
					blocks.Add(currentBlock);
					if (statement.GetType() == typeof(Goto))
					{
						currentBlock.Statements.Add(statement);
						currentBlock = new Block(this);
						currentBlock.Declarations = this.Declarations;
						currentBlock.Function = this.Function;
						currentBlock.stack = stack;
					}
					else
					{
						currentBlock = new Block(this);
						currentBlock.Declarations = this.Declarations;
						currentBlock.Function = this.Function;
						currentBlock.stack = stack;
						currentBlock.Statements.Add(statement);
					}
				}
				else
				{
					currentBlock.Statements.Add(statement);
				}
			}
			blocks.Add(currentBlock);
			return blocks;
		}

        static public Block ParseBlock(ref List<Token>.Enumerator tokens, Block parent)
        {
            return ParseBlock(ref tokens, parent, null);
        }

        static public Block ParseBlock(ref List<Token>.Enumerator tokens, Block parent, FunctionType func)
        {
			var thisBlock = new Block(parent);

            thisBlock.Function = func;

            // Climb the parents to find the function we are in
            if (func == null)
            {
                Block curBlock = thisBlock;
                do
                {
                    if (curBlock.Function != null)
                    {
                        func = curBlock.Function;
                    }
                    curBlock = curBlock.Parent;
                }
                while (curBlock != null);
            }
            
            if (parent != null)
            {
                thisBlock.TempDeclarationNumber = parent.TempDeclarationNumber;
            }

            while (tokens.Current != null && tokens.Current.Type != TokenType.CloseBlock)
            {
                if (tokens.Current.Text == "typedef")
                {

                }
                else if (tokens.Current.Text == "if")
                {
                    thisBlock.Statements.Add(If.ParseIf(thisBlock, ref tokens));
                }
				else if (tokens.Current.Text == "do")
				{
					thisBlock.Statements.Add(DoWhile.Parse(thisBlock, ref tokens));
				}
				else if (tokens.Current.Text == "while")
				{
					thisBlock.Statements.Add(While.Parse(thisBlock, ref tokens));
				}
				else if (tokens.Current.Text == "struct")
				{
					tokens.MoveNext();
					if (tokens.Current.Type != TokenType.OpenBlock)
					{
						throw new System.Exception("That was gay");
					}
				}
				else if (tokens.Current.Text == "return")
				{
					tokens.MoveNext();

					if (tokens.Current.Type == TokenType.StatementEnd)
					{
						var returnStatement = new Return(null);
						thisBlock.Statements.Add(returnStatement);
						tokens.MoveNext();
					}
					else
					{
						var returnList = Tokenizer.GetStatement(ref tokens);
						Declaration decl = thisBlock.CreateTempDeclaration(func.ReturnType);

						StatementHelper.Parse(thisBlock, decl, returnList);

						var returnStatement = new Return(decl);
						thisBlock.Statements.Add(returnStatement);

						tokens.MoveNext();
					}
				}
				else
				{
                    FunctionType.CallConvention specifiedConvention = FunctionType.CallConvention.CalleeSave;
					bool useStack = false;
					while (tokens.Current.Text.StartsWith("__"))
					{
						switch (tokens.Current)
						{
							case "__usestack":
								useStack = true;
								break;
							case "__stdcall":
								specifiedConvention = FunctionType.CallConvention.CalleeSave;
								break;
							case "__cdecl":
								specifiedConvention = FunctionType.CallConvention.CallerSave;
								break;
						}
						tokens.MoveNext();
					}

					Declaration declForStatement = thisBlock.FindDeclaration(tokens.Current.Text);
					if (declForStatement != null)
					{
						StatementHelper.Parse(thisBlock, Tokenizer.GetStatement(ref tokens));
						Debug.Assert(tokens.Current.Type == TokenType.StatementEnd);
						tokens.MoveNext();
					}
					else
					{
						String resultName = "";
						Type resultType = TypeHelper.ParseType(ref tokens);
						if (resultType == null)
						{
							StatementHelper.Parse(thisBlock, Tokenizer.GetStatement(ref tokens));
							Debug.Assert(tokens.Current.Type == TokenType.StatementEnd);
							tokens.MoveNext();
						}
						else
						{
							// Read the name of the declaration/type
							if (tokens.Current.Type == TokenType.StringType)
							{
								resultName = tokens.Current.Text;
								tokens.MoveNext();
							}

							// in this case it's either a prototype or a function
							// Either way it gets added to the types of this module
							if (tokens.Current.Type == TokenType.OpenParen)
							{
								FunctionType function = new FunctionType(ref tokens, resultType)
									{
										CallingConvention = specifiedConvention,
										UseStack = useStack
									};
								resultType = function;
								thisBlock.Types.Add(function);

								if (tokens.Current.Type == TokenType.OpenBlock)
								{
									var declaration = new Declaration(resultType, resultName);
									thisBlock.Declarations.Add(declaration);

									tokens.MoveNext();
									var block = Block.ParseBlock(ref tokens, thisBlock, function);

									Debug.Assert(tokens.Current.Type == TokenType.CloseBlock);
									tokens.MoveNext();

									declaration.Code = block;
									//STP: Not sure about this
									//thisBlock.Declarations.Add(declaration);
								}
								else
								{
									Debug.Assert(tokens.Current.Type == TokenType.StatementEnd);
									tokens.MoveNext();
								}
							}
							else if (tokens.Current == "[")
							{
								resultType = new WabbitC.Model.Types.Array(resultType, ref tokens);
								var declaration = new Declaration(resultType, resultName);
								thisBlock.Declarations.Add(declaration);
								Debug.Assert(tokens.Current.Type == TokenType.StatementEnd);
								tokens.MoveNext();
							}
							else
							{
								var decl = new Declaration(resultType, resultName);
								thisBlock.Declarations.Add(decl);

								// Handle declarations with initial values
								if (tokens.Current.Text == "=")
								{
									tokens.MoveNext();

									var valueList = Tokenizer.GetStatement(ref tokens);
									StatementHelper.Parse(thisBlock, decl, valueList);

									Debug.Assert(tokens.Current.Type == TokenType.StatementEnd);
									tokens.MoveNext();
								}
								else
								{
									//TODO: check for comma operator
									Debug.Assert(tokens.Current.Type == TokenType.StatementEnd);
									tokens.MoveNext();
								}
							}
						}
					}
				}
            }
            return thisBlock;
        }

        public Block()
        {
            Declarations = new List<Declaration>();
            Types = new HashSet<Type>();
            Statements = new BlockStatements(this);
        }

		public Block(Block parent) : this()
		{
			this.Parent = parent;
		}

        public override string ToString()
        {
            string result = "";
            if (this.GetType() != typeof(Module))
            {
                result += "{\n";
            }

            foreach (Declaration decl in Declarations)
            {
                result += decl.ToDeclarationString() + "\n";
            }

            if (Statements != null)
            {
                foreach (Statement statement in Statements)
                {
                    result += statement + "\n";
                }
            }
            if (this.GetType() != typeof(Module))
            {
                result += "}\n";
            }
            return result;
        }

		public string ToAssemblyString() 
		{
			var sb = new StringBuilder();
//			string tabs = "\t";
			foreach (var statement in Statements)
			{
				var asmString = statement.ToAssemblyString();
				if (asmString == "")
					continue;
				if (statement.GetType() != typeof(Label) && asmString.Length > 0 && asmString[0] != '\t')
					sb.Append("\t");
				//asmString = asmString.Replace(Environment.NewLine, Environment.NewLine + tabs);
				sb.AppendLine(asmString);
				/*if (statement.GetType() == typeof(Push))
					tabs += "\t";
				else if (statement.GetType() == typeof(Pop))
					tabs = tabs.Remove(tabs.Length - 1);*/
			}
			return sb.ToString();
		}

        #region IEnumerable Members

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            throw new NotImplementedException();
        }

        #endregion

        #region IEnumerable<Statement> Members

        IEnumerator<Statement> IEnumerable<Statement>.GetEnumerator()
        {
            List<Statement> listStatements = new List<Statement>();
            BlockEnumerator blockEnum = new BlockEnumerator(this);
            while (blockEnum.MoveNext())
            {
                listStatements.AddRange(blockEnum.Current.Statements);
            }
            return listStatements.GetEnumerator();
        }

        #endregion

        #region IEnumerable<Block> Members

        IEnumerator<Block> IEnumerable<Block>.GetEnumerator()
        {
            return new BlockEnumerator(this);
        }

        #endregion
    }
}
