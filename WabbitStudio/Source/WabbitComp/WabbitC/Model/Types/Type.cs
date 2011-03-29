using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

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
            }
            else
            {
                MessageSystem.Instance.ThrowNewError("Cannot dereference");
            }
        }

        public void Reference()
        {
            IndirectionLevels++;
        }

        public Type()
        {
            IndirectionLevels = 0;
        }

        public virtual object Clone()
        {
            return this.MemberwiseClone();
        }

        
    }
}
