namespace Revsoft.Wabbitcode.Services.Symbols
{
    public class CalcLocation
    {
        public CalcLocation(ushort address, byte page)
        {
            Address = address;
            Page = page;
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
		    return Address == other.Address && Page == other.Page;
	    }

	    public override int GetHashCode()
	    {
			return (Address.GetHashCode()*397) ^ Page.GetHashCode();
	    }

	    public override string ToString()
        {
            return "Page: " + Page + " Address: " + Address.ToString("X").ToUpper();
        }
    }

}
