using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model
{
    abstract class Type : ICloneable
    {
        private int size;
        public int Size
        {
            get
            {
                return size;
            }
            set
            {
                size = value;
            }
        }

        public int IndirectionLevels;

        public void Dereference()
        {
            if (IndirectionLevels > 0)
            {
                IndirectionLevels--;
            }
            else
            {
                MessageSystem.Instance.ThrowNewError("Cannot deference");
            }
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
