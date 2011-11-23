using System;
using System.Windows.Controls;
using AvalonDock;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Interface.Services;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Utilities;

namespace Revsoft.Wabbitcode.Panels
{
	/// <summary>
	/// Interaction logic for ProjectViewer.xaml
	/// </summary>
	public partial class ProjectViewer : DockableContent, IWabbitcodePanel
	{
		IProjectService projectService;
		public ProjectViewer()
		{
			projectService = ServiceFactory.Instance.GetServiceInstance<ProjectService>();
			InitializeComponent();
			projectService.ProjectOpened += ProjectService_ProjectOpened;
		}

		void ProjectService_ProjectOpened(object sender, EventArgs e)
		{
			BuildProjTree();
		}

		public void BuildProjTree()
		{
			projViewer.Items.Clear();
			//projViewer.TreeViewNodeSorter = new NodeSorter();
			IProjectFolder folder = projectService.CurrentProject.MainFolder;
			RecurseAddNodes(folder, null);
			if (projViewer.Items.Count > 0)
				(projViewer.Items[0] as TreeViewItem).ExpandSubtree();
			//projViewer.Sort();
		}

		private void RecurseAddNodes(IProjectFolder folder, TreeViewItem parentNode)
		{
			var nodeAdded = AddFolder(folder, parentNode);
			foreach (IProjectFolder subFolder in folder.Folders)
				RecurseAddNodes(subFolder, nodeAdded);
			foreach (IProjectFile file in folder.Files)
				AddFile(file, nodeAdded);
		}

		private TreeViewItem AddFolder(IProjectFolder folder, TreeViewItem parent)
		{
			var nodeFolder = new TreeViewItem
			{
				Tag = folder,
				Header = folder.Name,
			};

			if (parent == null)
				projViewer.Items.Add(nodeFolder);
			else
				parent.Items.Add(nodeFolder);
			return nodeFolder;
		}

		public void AddFile(IProjectFile file, TreeViewItem parent)
		{
			var nodeFile = new TreeViewItem
			{
				Tag = file,
				Header = System.IO.Path.GetFileName(file.FileFullPath),
			};
			if (parent == null)
				projViewer.Items.Add(nodeFile);
			else
				parent.Items.Add(nodeFile);
		}

		public void Cut()
		{
			throw new NotImplementedException();
		}

		public void Copy()
		{
			throw new NotImplementedException();
		}

		public void Paste()
		{
			throw new NotImplementedException();
		}

		public void Undo()
		{
			throw new NotImplementedException();
		}

		public void Redo()
		{
			throw new NotImplementedException();
		}
	}
}
