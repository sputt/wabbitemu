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
        public int TempDeclarationNumber = 0;

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

        static public Block ParseBlock(ref List<Token>.Enumerator tokens, Block parent)
        {
            return ParseBlock(ref tokens, parent, null);
        }

        static public Block ParseBlock(ref List<Token>.Enumerator tokens, Block parent, FunctionType func)
        {
            var thisBlock = new Block();
            thisBlock.Parent = parent;

            thisBlock.Function = func;

            // Climp the parents to find the function we are in
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
								FunctionType function = new FunctionType(ref tokens, resultType);
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
