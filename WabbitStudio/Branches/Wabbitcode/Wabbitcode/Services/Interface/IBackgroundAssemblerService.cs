using Revsoft.Wabbitcode.Services.Interface;

namespace Revsoft.Wabbitcode.Services
{
	public interface IBackgroundAssemblerService : IService
	{
		void RequestAssemble();
	}
}