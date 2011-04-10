using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Reflection;
using System.Diagnostics;

using WabbitC.Model.Types;

namespace WabbitC.Model.Statements.Math
{
    class MathStatement : ValueStatement
    {
        public Declaration LValue;
        public Token Operator;
        public Datum RValue;

        /// <summary>
        /// Builds the statements that generate the value of this expression
        /// </summary>
        /// <param name="block">Block to which to add the statements</param>
        /// <param name="expr">Expression to build</param>
        /// <param name="operand1">First operand</param>
        /// <param name="operand2">second operand</param>
        /// <returns></returns>
        public static Declaration BuildStatements(Block block, Expression expr, Token operand1, Token operand2)
        {
            Token opToken = expr.Tokens[0];

            var decl = block.CreateTempDeclaration(TypeHelper.GetType(block, operand1));
            ValueStatement initialAssign = AssignmentHelper.ParseSingle(block, decl, operand1);
            block.Statements.Add(initialAssign);

            var asm = System.Reflection.Assembly.GetExecutingAssembly();
            foreach (var type in asm.GetTypes())
            {
                if (type.BaseType == typeof(MathStatement))
                {
                    try
                    {
                        IMathOperator mathOp = (IMathOperator)asm.CreateInstance(type.FullName);
                        if (opToken.Equals(mathOp.GetHandledOperator()))
                        {
                            MathStatement mathStatement;
                            Datum datum = Datum.Parse(block, operand2);
							if (datum == null)
							{
								MessageSystem.Instance.ThrowNewError(MessageSystem.ErrorCode.UndeclaredVar);
								return null;
							}

                            if (decl.Type.IndirectionLevels > 0)
                            {
                                Type derefType = decl.Type.Clone() as Type;
                                derefType.Dereference();

                                var offsetDecl = block.CreateTempDeclaration(new BuiltInType("int"));
                                block.Statements.Add(AssignmentHelper.ParseSingle(block, offsetDecl, operand2));
                                var imm = new Immediate(new Token(derefType.Size.ToString()));
                                Mult mult = new Mult(offsetDecl, imm);
                                block.Statements.Add(mult);
                                datum = offsetDecl;
                            }
                           
                            mathStatement = Activator.CreateInstance(type,
                               new object[] { decl, datum }) as MathStatement;
                            block.Statements.Add(mathStatement);
                            break;
                        }
                    }
                    catch (MissingMethodException) { }
                }
            }

            return decl;
        }

        public override ISet<Declaration> GetModifiedDeclarations()
        {
            return new HashSet<Declaration>() { LValue };
        }

		public override ISet<Declaration> GetReferencedDeclarations()
        {
            if (RValue != null && RValue.GetType() == typeof(Declaration))
            {
				return new HashSet<Declaration>() { LValue, RValue as Declaration };
            }
            else
            {
				return new HashSet<Declaration>() { LValue };
            }
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder(LValue.Name);
            sb.Append(" ");
            if (RValue != null)
            {
                if (LValue.Type.IndirectionLevels > 0)
                {
                    sb.Append("= (" + LValue.Type + ") ((unsigned char *)");
                    sb.Append(LValue);
                    sb.Append(" ");
                    sb.Append(Operator);
                    sb.Append(RValue);
                    sb.Append(");");
                }
                else
                {
                    sb.Append(Operator);
                    sb.Append("= ");
					sb.Append("(" + LValue.Type + ") ");
                    sb.Append(RValue);
                    sb.Append(";");
                }

            }
            else
            {
                sb.Append("= ");
                sb.Append(Operator);
                sb.Append(LValue);
                sb.Append(";");
            }
            return sb.ToString();
        }
    }
}
