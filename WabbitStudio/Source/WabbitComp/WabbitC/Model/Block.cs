using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using WabbitC.Model.Types;
using WabbitC.Model.Statements;

namespace WabbitC.Model
{
    class Block
    {
        public Block Parent;
        public HashSet<Type> Types;
        public List<Declaration> Declarations;
        public List<Statement> Statements;
        public int TempDeclarationNumber = 0;

        public Declaration FindDeclaration(String name)
        {
            Block curBlock = this;
            while (curBlock != null)
            {
                foreach (Declaration decl in curBlock.Declarations)
                {
                    if (decl.Name == name)
                    {
                        return decl;
                    }
                }
                curBlock = curBlock.Parent;
            }
            return null;
        }

        public Declaration CreateTempDeclaration(Type type)
        {
            var declaration = new Declaration(;
            declaration.Name = "Temp" + TempDeclarationNumber;
        }

        static public Block ParseBlock(ref List<Token>.Enumerator tokens, Block parent)
        {
            var thisBlock = new Block();
            thisBlock.Parent = parent;
            thisBlock.Declarations = new List<Declaration>();
            thisBlock.Types = new HashSet<Type>();
            thisBlock.Statements = new List<Statement>();

            while (tokens.Current != null && tokens.Current.Type != TokenType.CloseBlock)
            {
                if (tokens.Current.Text == "typedef")
                {

                }
                else if (tokens.Current.Text == "struct")
                {
                    tokens.MoveNext();
                    if (tokens.Current.Type != TokenType.OpenBlock)
                    {
                        throw new System.Exception("That was gay");
                    }
                }

                Declaration declForStatement = thisBlock.FindDeclaration(tokens.Current.Text);
                if (declForStatement != null)
                {
                    // We have some kind of statement in this case
                    tokens.MoveNext();

                    if (tokens.Current.Text == "=")
                    {
                        tokens.MoveNext();
                        
                        var statement = new Assignment();
                        statement.LValue = declForStatement;
                        statement.RValue = new Value(new Immediate(tokens.Current));

                        thisBlock.Statements.Add(statement);

                        tokens.MoveNext();

                        Debug.Assert(tokens.Current.Type == TokenType.StatementEnd);
                        tokens.MoveNext();
                    }
                }
                else
                {
                    String resultName = "";
                    Type resultType = TypeHelper.ParseType(ref tokens);
                    if (resultType == null)
                    {
                        throw new System.Exception("Could not parse a type");
                    }

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
                            tokens.MoveNext();
                            var block = Block.ParseBlock(ref tokens, thisBlock);

                            Debug.Assert(tokens.Current.Type == TokenType.CloseBlock);
                            tokens.MoveNext();

                            var declaration = new Declaration(resultType, resultName);
                            declaration.Code = block;
                            thisBlock.Declarations.Add(declaration);
                        }
                        else
                        {
                            Debug.Assert(tokens.Current.Type == TokenType.StatementEnd);
                            tokens.MoveNext();
                        }
                    }
                    else
                    {


                        Debug.Assert(tokens.Current.Type == TokenType.StatementEnd);
                        tokens.MoveNext();

                        thisBlock.Declarations.Add(new Declaration(resultType, resultName));
                    }
                }
                
            }
            return thisBlock;
        }
    }
}
