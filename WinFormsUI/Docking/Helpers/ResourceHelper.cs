using System;
using System.Drawing;
using System.Reflection;
using System.Resources;
using System.Windows.Forms;

namespace Revsoft.Docking
{
	internal static class ResourceHelper
	{
        private static ResourceManager _resourceManager = null;

        private static ResourceManager ResourceManager
        {
            get
            {
                if (_resourceManager == null)
                    _resourceManager = new ResourceManager("Revsoft.Docking.Strings", typeof(ResourceHelper).Assembly);
                return _resourceManager;
            }

        }

		public static string GetString(string name)
		{
			return ResourceManager.GetString(name);
		}
	}
}
