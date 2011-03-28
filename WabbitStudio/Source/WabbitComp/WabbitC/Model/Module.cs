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
        private Module(Block block)
        {
            this.Types = block.Types;
            this.Declarations = block.Declarations;

            this.Declarations.InsertRange(0, new List<Declaration>()
            {
                new Declaration(new BuiltInType("int"), "__hl"),
                new Declaration(new BuiltInType("int"), "__de"),
                new Declaration(new BuiltInType("int"), "__bc"),
            });

			//TODO: make so assignment statements work...
            this.Parent = null;
        }

        public IEnumerator<Declaration> GetFunctionEnumerator()
        {
            var functions = from d in Declarations
                            where d.Type.GetType() == typeof(FunctionType) && d.Code != null
                            select d;
            return functions.GetEnumerator();
        }

        static public Module ParseModule(ref List<Token>.Enumerator tokens)
        {
            var block = Block.ParseBlock(ref tokens, null, null);
            // Make sure there's no stuff that doesn't belong in module
            return new Module(block);
        }
    }
}
