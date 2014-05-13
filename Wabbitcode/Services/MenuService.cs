using System;
using System.Collections.Generic;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public class MenuService : IMenuService
    {
        private readonly Dictionary<string, MenuStrip> _menus = new Dictionary<string, MenuStrip>();
        private readonly ToolStripPanel _panel;

        public MenuService(ToolStripPanel panel)
        {
            _panel = panel;
        }

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
    }
}