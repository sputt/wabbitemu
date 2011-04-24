using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using AvalonDock;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode.Panels
{
    /// <summary>
    /// Interaction logic for ProjectViewer.xaml
    /// </summary>
    public partial class ProjectViewer : DockableContent, IWabbitcodePanel
    {
        public ProjectViewer()
        {
            InitializeComponent();
            ProjectService.ProjectOpened += ProjectService_ProjectOpened;
        }

        void ProjectService_ProjectOpened(object sender, EventArgs e)
        {
            BuildProjTree();
        }

        public void BuildProjTree()
        {
            projViewer.Items.Clear();
            //projViewer.TreeViewNodeSorter = new NodeSorter();
            ProjectFolder folder = ProjectService.CurrentProject.MainFolder;
            RecurseAddNodes(folder, null);
            if (projViewer.Items.Count > 0)
                (projViewer.Items[0] as TreeViewItem).ExpandSubtree();
            //projViewer.Sort();
        }

        private void RecurseAddNodes(ProjectFolder folder, TreeViewItem parentNode)
        {
            var nodeAdded = AddFolder(folder, parentNode);
            foreach (ProjectFolder subFolder in folder.Folders)
                RecurseAddNodes(subFolder, nodeAdded);
            foreach (ProjectFile file in folder.Files)
                AddFile(file, nodeAdded);
        }

        private TreeViewItem AddFolder(ProjectFolder folder, TreeViewItem parent)
        {
            var nodeFolder = new TreeViewItem
            {
                Tag = folder,
                Header = folder.Name,
                /*ImageIndex = 2,
                SelectedImageIndex = 3,*/
            };
            if (parent == null)
                projViewer.Items.Add(nodeFolder);
            else
                parent.Items.Add(nodeFolder);
            return nodeFolder;
        }

        public void AddFile(ProjectFile file, TreeViewItem parent)
        {
            var nodeFile = new TreeViewItem
            {
                Tag = file,
                Header = System.IO.Path.GetFileName(file.FileFullPath),
                /*ImageIndex = 4,
                SelectedImageIndex = 5,*/
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
