using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Model
{
    class Value
    {
        enum ValueType
        {
            Variable,
            Immediate,
        };

        private object value;
        public object Val
        {
            get
            {
                return value;
            }
            set
            {
                if (value.GetType() != typeof(Declaration) &&
                    value.GetType() != typeof(Immediate))
                {
                    throw new System.InvalidCastException();
                }
                this.value = value;
            }
        }

        public Value(Immediate val)
        {
            Val = val;
        }
        public Value(Declaration val)
        {
            Val = val;
        }

        public override string ToString()
        {
            return Val.ToString();
        }
    }
}
