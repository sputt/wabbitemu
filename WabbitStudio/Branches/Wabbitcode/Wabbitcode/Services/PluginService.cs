using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
    internal class PluginService : IPluginService
    {
        private readonly List<IWabbitcodePlugin> _plugins = new List<IWabbitcodePlugin>();

        public void LoadPlugins()
        {
            string dir = Path.GetDirectoryName(Application.ExecutablePath);
            if (dir == null)
            {
                return;
            }

            string[] dllFileNames = Directory.GetFiles(dir, "*.dll");

            List<Assembly> assemblies = new List<Assembly>();
            assemblies.AddRange(dllFileNames.Select(AssemblyName.GetAssemblyName).Select(Assembly.Load));

            Type pluginType = typeof(IWabbitcodePlugin);
            ICollection<Type> pluginTypes = assemblies.Where(a => a != null)
                .Select(assembly => assembly.GetTypes())
                .SelectMany(types => types
                    .Where(type => !type.IsInterface && !type.IsAbstract && type.GetInterface(pluginType.FullName) != null))
                .ToList();

            foreach (IWabbitcodePlugin plugin in 
                pluginTypes.Select(type => (IWabbitcodePlugin)Activator.CreateInstance(type)))
            {
                _plugins.Add(plugin);
                plugin.Loaded();
            }
        }

        public void UnloadPlugins()
        {
            foreach (var plugin in _plugins)
            {
                plugin.Unloaded();
            }
        }

        public void DestroyService()
        {
        }

        public void InitService(params object[] objects)
        {
        }

    }

    public interface IWabbitcodePlugin
    {
        void Loaded();

        void Unloaded();
    }
}
