using System;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public class ListFileValue : IEquatable<ListFileValue>
    {
        public ListFileValue(ushort address, byte page)
        {
            this.Address = address;
            this.Page = page;
        }

        public ushort Address
        {
            get;
            private set;
        }

        public byte Page
        {
            get;
            private set;
        }

        public override bool Equals(object obj)
        {
            if (obj.GetType() != typeof(ListFileValue))
            {
                return false;
            }
            ListFileValue value = obj as ListFileValue;
            return value.Page == this.Page && value.Address == this.Address;
        }

        public bool Equals(ListFileValue value)
        {
            return value.Page == this.Page && value.Address == this.Address;
        }

        public override int GetHashCode()
        {
            return this.Page + this.Address;
        }

        public override string ToString()
        {
            return "Page: " + this.Page + " Address: " + this.Address.ToString("X");
        }
    }

}
