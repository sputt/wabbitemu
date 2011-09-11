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

		static readonly RoutedUICommand aboutCommand = new RoutedUICommand("About", "About", typeof(RoutedUICommand));
		public static RoutedUICommand About { get { return aboutCommand; } }

		static readonly RoutedUICommand refactorRenameCommand = new RoutedUICommand("Rename", "Rename", typeof(RoutedUICommand));
		public static RoutedUICommand RefactorRename { get { return refactorRenameCommand; } }

		static readonly RoutedUICommand refactorExtractMethodCommand = new RoutedUICommand("Extract Method", "Extract Method", typeof(RoutedUICommand));
		public static RoutedUICommand RefactorExtractMethod { get { return refactorExtractMethodCommand; } }

		static readonly RoutedUICommand findAllReferencesCommand = new RoutedUICommand("Find All References", "Find All References", typeof(RoutedUICommand));
		public static RoutedUICommand FindAllReferences { get { return findAllReferencesCommand; } }
	}
}
