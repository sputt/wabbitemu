using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;

using WabbitC.Model.Types;

namespace WabbitC.Model
{
    class Module : Block
    {
		public List<string> IntermediateStrings;

        private Module(Block block)
        {
            this.Types = block.Types;
            this.Declarations = block.Declarations;
			IntermediateStrings = new List<string>();

            this.Declarations.InsertRange(0, new List<Declaration>()
            {
                new Declaration(new BuiltInType("int"), "__hl"),
                new Declaration(new BuiltInType("int"), "__de"),
                new Declaration(new BuiltInType("int"), "__bc"),
				new Declaration(new BuiltInType("int"), "__iy"),
				new Declaration(new BuiltInType("int"), "__ix"),
				new Declaration(new BuiltInType("int"), "__h"),
				new Declaration(new BuiltInType("int"), "__l"),
				new Declaration(new BuiltInType("int"), "__d"),
				new Declaration(new BuiltInType("int"), "__e"),
				new Declaration(new BuiltInType("int"), "__b"),
				new Declaration(new BuiltInType("int"), "__c"),
				new Declaration(new BuiltInType("int"), "__a"),
				new Declaration(new Types.Array(new BuiltInType("unsigned char"), "[2048]"), "__stack"),
				new Declaration(new BuiltInType("int"), "__sp"),
            });

			//TODO: make so assignment statements work...
            this.Parent = null;
        }

        public IEnumerator<Declaration> GetFunctionEnumerator()
        {
            var functions = from d in Declarations
                            where (d.Type.GetType() == typeof(FunctionType) || d.Type.GetType().BaseType == typeof(FunctionType)) && d.Code != null
                            select d;
            return functions.GetEnumerator();
        }

        static public Module ParseModule(ref List<Token>.Enumerator tokens)
        {
            var block = Block.ParseBlock(ref tokens, null, null);
            // Make sure there's no stuff that doesn't belong in module
            return new Module(block);
        }

		public override string ToString()
		{
			string result = "";
			foreach (string s in IntermediateStrings)
			{
				result += s + "\n";
			}
			result += base.ToString();
			return result;
		}
    }
}
