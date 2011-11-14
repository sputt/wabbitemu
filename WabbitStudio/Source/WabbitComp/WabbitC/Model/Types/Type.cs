using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using WabbitC.Model.Types;

namespace WabbitC.Model
{
    abstract class Type : ICloneable
    {
        public int Size;

        public abstract string ToDeclarationString(string DeclName);
        public int IndirectionLevels;

        public void Dereference()
        {
            if (IndirectionLevels > 0)
            {
                IndirectionLevels--;
				List<Token> tokenList = Tokenizer.Tokenize(this.ToString());
				var tokens = tokenList.GetEnumerator();
				tokens.MoveNext();
				Size = TypeHelper.ParseType(ref tokens).Size;
            }
            else
            {
                MessageSystem.Instance.ThrowNewError("Cannot dereference");
            }
        }

        public void Reference()
        {
            IndirectionLevels++;
			Size = new BuiltInType("void *").Size;
        }

        public Type()
        {
            IndirectionLevels = 0;
        }

        public virtual object Clone()
        {
            return this.MemberwiseClone();
        }

        public override bool Equals(object obj)
        {
            if (!(obj is Type))
                return base.Equals(obj);
            var type = (Type) obj;
            return type.IndirectionLevels == this.IndirectionLevels && this.Size == type.Size;
        }
        
    }
}
