namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public interface IPluginService : IService
    {
        void LoadPlugins();

        void UnloadPlugins();
    }
}
