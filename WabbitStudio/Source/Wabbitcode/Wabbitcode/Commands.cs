using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Input;

namespace Revsoft.Wabbitcode
{
    public static class WabbitcodeCommands
    {
        static readonly RoutedUICommand newProject = new RoutedUICommand("New Project", "NewProject", typeof(RoutedUICommand));
        public static RoutedUICommand NewProject { get { return newProject; } }
        
    }
}
