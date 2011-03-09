using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using WabbitC.Model.Types;
using WabbitC.Model.Statements;

namespace WabbitC.Model
{
    public class Block
    {
        public Block Parent;
        public FunctionType Function;
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
                if (Function != null)
                {
                    foreach (FunctionType.ParamDef param in Function.ParamDefs)
                    {
                        if (param.Name == name)
                        {
                            return new Declaration(param.Type, name);
                        }
                    }
                }
                curBlock = curBlock.Parent;
            }
            return null;
        }

        public Declaration CreateTempDeclaration(Type type)
        {
            var declaration = new Declaration(type, "__temp" + TempDeclarationNumber);
            Declarations.Add(declaration);

            Block curBlock = this;
            while (curBlock != null)
            {
                curBlock.TempDeclarationNumber++;
                curBlock = curBlock.Parent;
            }
            
			return declaration;
        }

        static public Block ParseBlock(ref List<Token>.Enumerator tokens, Block parent)
        {
            return ParseBlock(ref tokens, parent, null);
        }

        static public Block ParseBlock(ref List<Token>.Enumerator tokens, Block parent, FunctionType func)
        {
            var thisBlock = new Block();
            thisBlock.Function = func;
            thisBlock.Parent = parent;
            if (parent != null)
            {
                thisBlock.TempDeclarationNumber = parent.TempDeclarationNumber;
            }
            thisBlock.Declarations = new List<Declaration>();
            thisBlock.Types = new HashSet<Type>();
            thisBlock.Statements = new List<Statement>();

            while (tokens.Current != null && tokens.Current.Type != TokenType.CloseBlock)
            {
                if (tokens.Current.Text == "typedef")
                {

                }
                else if (tokens.Current.Text == "if")
                {
                    thisBlock.Statements.Add(If.ParseIf(thisBlock, ref tokens));
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

                    var returnList = Tokenizer.GetStatement(ref tokens);
                    Declaration decl = thisBlock.CreateTempDeclaration(func.ReturnType);

                    AssignmentHelper.Parse(thisBlock, decl, returnList);

                    var returnStatement = new Return(decl);
                    thisBlock.Statements.Add(returnStatement);

                    tokens.MoveNext();
                }
                else
                {
                    Declaration declForStatement = thisBlock.FindDeclaration(tokens.Current.Text);
                    if (declForStatement != null)
                    {
                        // We have some kind of statement in this case
                        tokens.MoveNext();

                        if (tokens.Current.Text == "=")
                        {
                            tokens.MoveNext();

                            var valueList = Tokenizer.GetStatement(ref tokens);
                            var expr = new Expression(valueList);

                            AssignmentHelper.Parse(thisBlock, declForStatement, valueList);

                            Debug.Assert(tokens.Current.Type == TokenType.StatementEnd);
                            tokens.MoveNext();
                        }
                        else if (tokens.Current.Text == "(")
                        {
                            tokens.MoveNext();
                            List<Declaration> paramList = FunctionCall.BuildParams(thisBlock, (FunctionType)declForStatement.Type, ref tokens);

                            Declaration returnDecl;
                            Type returnType = ((FunctionType)declForStatement.Type).ReturnType;
                            if (returnType.Equals(new BuiltInType("void")))
                            {
                                returnDecl = null;
                            }
                            else
                            {
                                returnDecl = thisBlock.CreateTempDeclaration(returnType);
                            }
                            var funcCall = new FunctionCall(returnDecl, declForStatement, paramList);
                            thisBlock.Statements.Add(funcCall);

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
                            // Dereference of some kind
                            if (tokens.Current == "*")
                            {
                                Store.Parse(thisBlock, ref tokens);
                            }
                            else
                            {
                                throw new System.Exception("Not a known statement");
                            }
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
                                var block = Block.ParseBlock(ref tokens, thisBlock, function);

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
                            var decl = new Declaration(resultType, resultName);
                            thisBlock.Declarations.Add(decl);

                            // Handle declarations with initial values
                            if (tokens.Current.Text == "=")
                            {
                                tokens.MoveNext();

                                var valueList = Tokenizer.GetStatement(ref tokens);
                                AssignmentHelper.Parse(thisBlock, decl, valueList);

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
            return thisBlock;
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
                result += decl.GetDeclaration() + "\n";
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
    }
}
