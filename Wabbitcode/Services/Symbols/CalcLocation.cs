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

        public override string ToString()
        {
            return "Page: " + Page + " Address: " + Address.ToString("X").ToUpper();
        }
    }

}
