namespace Revsoft.Wabbitcode.Services.Symbols
{
    public class CalcLocation
    {
        public CalcLocation(ushort address, byte page, bool isRam)
        {
            Address = address;
            Page = page;
            IsRam = isRam;
        }

        public ushort Address { get; private set; }

        public byte Page { get; private set; }

        public bool IsRam { get; private set; }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj))
            {
                return false;
            }

            if (ReferenceEquals(this, obj))
            {
                return true;
            }

            return obj.GetType() == GetType() && Equals((CalcLocation) obj);
        }

        private bool Equals(CalcLocation other)
        {
            return Address == other.Address && Page == other.Page && IsRam == other.IsRam;
        }

        public override int GetHashCode()
        {
            return (IsRam.GetHashCode() * 397) ^ (Address.GetHashCode() * 397) ^ Page.GetHashCode();
        }

        public override string ToString()
        {
            return "Page: " + Page + " Address: " + Address.ToString("X").ToUpper() + " IsRam: " + IsRam;
        }
    }
}