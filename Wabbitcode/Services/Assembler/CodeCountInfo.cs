namespace Revsoft.Wabbitcode.Services.Assembler
{
    public class CodeCountInfo
    {
        public int Size { get; private set; }
        public int Min { get; private set; }
        public int Max { get; private set; }

        public CodeCountInfo(int size, int min, int max)
        {
            Size = size;
            Min = min;
            Max = max;
        }
    }
}
