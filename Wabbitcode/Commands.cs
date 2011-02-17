using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Input;

namespace Revsoft.Wabbitcode
{
    public static class WabbitcodeCommands
    {
        static readonly RoutedUICommand newProjectCommand = new RoutedUICommand("New Project", "NewProject", typeof(RoutedUICommand));
        public static RoutedUICommand NewProject { get { return newProjectCommand; } }

        static readonly RoutedUICommand openProjectCommand = new RoutedUICommand("Open Project", "OpenProject", typeof(RoutedUICommand));
        public static RoutedUICommand OpenProject { get { return openProjectCommand; } }

        static readonly RoutedUICommand saveAllCommand = new RoutedUICommand("Save All", "SaveAll", typeof(RoutedUICommand));
        public static RoutedUICommand SaveAll { get { return saveAllCommand; } }

        static readonly RoutedUICommand assembleCommand = new RoutedUICommand("Assemble", "Assemble", typeof(RoutedUICommand));
        public static RoutedUICommand Assemble { get { return assembleCommand; } }
    }
}
