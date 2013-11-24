using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
    class MenuService : IMenuService
    {
        private readonly Dictionary<string, MenuStrip> _menus = new Dictionary<string, MenuStrip>();
        private ToolStripPanel _panel;

        public void RegisterMenu(string menuName, MenuStrip menu)
        {
            if (_menus.ContainsKey(menuName))
            {
                throw new ArgumentException("Menu is already registered");
            }

            _panel.Join(menu, 0);
        }

        public MenuStrip GetMenu(string menuName)
        {
            MenuStrip menu;
            _menus.TryGetValue(menuName, out menu);
            return menu;
        }

        public void DestroyService()
        {
        }

        public void InitService(params object[] objects)
        {
            if (objects.Length != 1)
            {
                throw new ArgumentException("Toolabar service expects exactly one param");
            }

            ToolStripPanel panel = objects.First() as ToolStripPanel;
            if (panel == null)
            {
                throw new ArgumentException("Expected first argumet to be a ToolStripPanel");
            }

            _panel = panel;
        }
    }
}
