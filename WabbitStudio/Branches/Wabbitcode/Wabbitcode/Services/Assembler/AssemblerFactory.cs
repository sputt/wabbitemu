namespace Revsoft.Wabbitcode.Services.Assembler
{
    internal static class AssemblerFactory
    {
        public static IAssembler CreateAssembler()
        {
            return new SpasmComAssembler();
        }
    }
}
