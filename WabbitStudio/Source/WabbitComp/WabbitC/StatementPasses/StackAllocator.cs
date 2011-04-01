using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model;
using WabbitC.Model.Statements;
using WabbitC.Model.Types;

namespace WabbitC.StatementPasses
{
    class StackAllocator
    {
		public Block Parent { get; private set; }

		List<Declaration> stack = new List<Declaration>();

		int nFixed = 0;
		int nOffset = 0;
		public int Offset
		{
			get { return nOffset; }
		}

		public int Size
		{
			get { return nOffset; }
		}
		
		public StackAllocator(Block parent)
		{
			Parent = parent;
		}

		public int ReserveSpace(Declaration decl)
		{
			int temp = nOffset;
			nOffset += decl.Type.Size;
			stack.Add(decl);
			return temp;
		}
		public int ReserveSpace(int bytes)
		{
			Declaration tempDecl = new Declaration(new WabbitC.Model.Types.Array(new BuiltInType("unsigned char"), "[" + bytes + "]"), "__FIXED_SIZE" + nFixed++);
			return ReserveSpace(tempDecl);
		}
		public int GetOffset(Declaration decl)
		{
			int offset = 0;
			foreach (var stackItem in stack)
			{
				if (stackItem == decl)
					break;
				offset += stackItem.Type.Size;
			}
			return offset;
		}
		
        /*public static void Run(Module module)
        {
            var functions = module.GetFunctionEnumerator();
            while (functions.MoveNext())
            {
                if (functions.Current.Code != null)
                {
                    int nOffset = 0;
                    foreach (Declaration decl in (functions.Current.Type as FunctionType).Params)
                    {
                        decl.StackOffset = nOffset;
                        nOffset += decl.Type.Size;
                    }

                    foreach (Declaration decl in functions.Current.Code.Declarations)
                    {
                        decl.StackOffset = nOffset;
                        nOffset += decl.Type.Size;
                    }
                }
            }
        }*/
    }
}
