namespace Revsoft.Wabbitcode.Services.Assembler
{
    public interface IAssemblerFactory
    {
        IAssembler CreateAssembler();
    }

    public class AssemblerFactory : IAssemblerFactory
    {
        public IAssembler CreateAssembler()
        {
            return new SpasmComAssembler();
        }
    }
}
