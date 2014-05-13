using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public interface IMenuService
    {
        void RegisterMenu(string menuName, MenuStrip menu);

        MenuStrip GetMenu(string menuName);
    }
}