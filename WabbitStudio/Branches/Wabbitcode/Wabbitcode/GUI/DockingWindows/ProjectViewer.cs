using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using Aga.Controls.Tree;
using Aga.Controls.Tree.NodeControls;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class ProjectViewer : ToolWindow
    {
        #region Private Members

        private readonly IProjectService _projectService;
        private ProjectTreeModel _model;

        #endregion

        public ProjectViewer()
        {
            InitializeComponent();

            _projectService = DependencyFactory.Resolve<IProjectService>();
            if (_projectService.Project != null)
            {
                CreateModel();
            }

            _projectService.ProjectOpened += OpenProjectTreeEvent;
            _projectService.ProjectClosed += CloseProjectTreeEvent;
            _projectService.ProjectFileAdded += BuildProjectTreeEvent;
            _projectService.ProjectFileRemoved += BuildProjectTreeEvent;
            _projectService.ProjectFolderAdded += BuildProjectTreeEvent;
            _projectService.ProjectFolderRemoved += BuildProjectTreeEvent;
        }

        private void OpenProjectTreeEvent(object sender, EventArgs args)
        {
            CreateModel();
            BuildProjTree();
        }

        private void CreateModel()
        {
            _model = new ProjectTreeModel(_projectService.Project.MainFolder);
            var sortedModel = new SortedTreeModel(_model) {Comparer = new NodeSorter()};
            projViewer.Model = sortedModel;
            projViewer.Root.Children.First().Expand();
        }

        private void BuildProjectTreeEvent(object sender, EventArgs args)
        {
            BuildProjTree();
        }

        private void CloseProjectTreeEvent(object sender, EventArgs args)
        {
            CloseProject();
        }

        private void NodeEditorShowing(object sender, CancelEventArgs e)
        {
            ProjectFolder folder = projViewer.CurrentNode.Tag as ProjectFolder;
            if (folder == _projectService.Project.MainFolder)
            {
                e.Cancel = true;
            }
        }

        private void AddFile(FilePath file, ProjectFolder folder)
        {
            ProjectFile fileAdded = _projectService.AddFile(folder, file);

            string filePath = fileAdded.FileFullPath;
            if (string.IsNullOrEmpty(filePath))
            {
                // should never happen
                return;
            }

            _model.OnNodesChanged(fileAdded.ParentFolder);
        }

        private void BuildProjTree()
        {
            _model.OnNodesChanged(_projectService.Project.MainFolder);
            projViewer.Root.Children.First().Expand();
        }

        private void DeleteNode(TreeNodeAdv node)
        {
            var folder = node.Tag as ProjectFolder;
            if (folder != null)
            {
                _projectService.DeleteFolder(folder.Parent, folder);
                _model.OnNodesChanged(folder);
            }
            else
            {
                var file = node.Tag as ProjectFile;
                if (file == null)
                {
                    return;
                }

                File.Delete(file.FileFullPath);
                _projectService.DeleteFile(file.ParentFolder, file);
                _model.OnNodesChanged(file.ParentFolder);
            }
        }

        private void DeleteNodes(ReadOnlyCollection<TreeNodeAdv> nodes)
        {
            if (nodes.Count < 1)
            {
                return;
            }

            string nodeName = "";
            var singleNode = nodes.Single();
            var projectFolder = singleNode.Tag as ProjectFolder;
            var projectFile = singleNode.Tag as ProjectFile;
            if (projectFile != null)
            {
                projectFolder = projectFile.ParentFolder;
                nodeName = projectFile.Name;
            }
            else if (projectFolder != null)
            {
                nodeName = projectFolder.Name;
            }

            string message = nodes.Count > 1 ? "Would you like to delete the selected files permanently?" :
                "Delete '" + nodeName + "' permanently?";

            DialogResult results = MessageBox.Show(message, "Delete Contents", MessageBoxButtons.YesNo, MessageBoxIcon.Question);

            if (results != DialogResult.Yes)
            {
                return;
            }

            foreach (TreeNodeAdv node in nodes)
            {
                if (node.Parent == null)
                {
                    string dir = _projectService.Project.ProjectFile;
                    _projectService.CloseProject();
                    File.Delete(dir);
                }
                else
                {
                    DeleteNode(node);
                }
            }

            _model.OnNodesChanged(projectFolder);
        }

        internal void AddExistingFile(FilePath file)
        {
            try
            {
                TreeNodeAdv parent = projViewer.SelectedNode;
                if (parent == null)
                {
                    parent = projViewer.Root.Children.First();
                }
                else if (parent.Tag is ProjectFile)
                {
                    parent = parent.Parent;
                }

                AddFile(file, (ProjectFolder)parent.Tag);
            }
            catch (Exception ex)
            {
                Services.DockingService.ShowError("Error adding file", ex);
            }
        }

        internal void AddNewFile(string fileName)
        {
            TreeNodeAdv parent = projViewer.SelectedNode;
            if (parent == null)
            {
                //parent = projViewer.Nodes[0];
            }
            else if (parent.Tag is ProjectFile)
            {
                parent = parent.Parent;
            }

            if (parent == null)
            {
                parent = projViewer.Root.Children.First();
            }

            FilePath file = _projectService.Project.ProjectDirectory.Combine(fileName);
            using (StreamWriter writer = File.CreateText(file))
            {
                writer.Close();
            }

            AddFile(file, (ProjectFolder) parent.Tag);
            AbstractUiAction.RunCommand(new OpenFileAction(file));
        }

        private void CloseProject()
        {
            //projViewer.Nodes.Clear();
        }

        private static void OpenAs(string file)
        {
            Process process = new Process
            {
                StartInfo =
                {
                    FileName = "rundll32.exe",
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    Arguments = "shell32.dll ,OpenAs_RunDLL " + file
                }
            };
            process.Start();
        }

        private void AddFolder(string folderName, ProjectFolder parentFolder)
        {
            ProjectFolder folder = new ProjectFolder(parentFolder, folderName);
            parentFolder.AddFolder(folder);
            _model.OnNodesChanged(parentFolder);
        }

        private void copyMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void cutMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void delFMenuItem_Click(object sender, EventArgs e)
        {
            DeleteNodes(projViewer.SelectedNodes);
        }

        private void delMenuItem_Click(object sender, EventArgs e)
        {
            //DeleteNodes(projViewer.SelectedNodes);
        }

        private void excFromProj_Click(object sender, EventArgs e)
        {
            foreach (TreeNodeAdv node in projViewer.SelectedNodes)
            {
                if (node.Tag is ProjectFolder)
                {
                    continue;
                }

                ProjectFile file = (ProjectFile) node.Tag;
                _projectService.DeleteFile(file.Folder, file);
            }
        }

        private void existingFileMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new AddExistingFileAction(this));
        }

        private void newFileContextItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new AddNewFileAction(this));
        }

        private void newFolderContextItem_Click(object sender, EventArgs e)
        {
            TreeNodeAdv parent = projViewer.SelectedNode ?? projViewer.AllNodes.First();

            if (parent.Tag is ProjectFile)
            {
                parent = parent.Parent;
            }

            RenameForm newNameForm = new RenameForm
            {
                Text = "New Folder"
            };

            if (newNameForm.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            string folderName = newNameForm.NewText;
            AddFolder(folderName, (ProjectFolder)parent.Tag);
        }

        private void openExplorerMenuItem_Click(object sender, EventArgs e)
        {
            Process explorer = new Process
            {
                StartInfo =
                {
                    FileName = CurrentFileName(projViewer.SelectedNode)
                }
            };
            explorer.Start();
        }

        private static string CurrentFileName(TreeNodeAdv node)
        {
            return ((ProjectFile) node.Tag).FileFullPath;
        }

        private void openMenuItem_Click(object sender, EventArgs e)
        {
            OpenNode(projViewer.SelectedNode);
            foreach (TreeNodeAdv node in projViewer.SelectedNodes)
            {
                OpenNode(node);
            }
        }

        private void OpenNode(TreeNodeAdv dropNode)
        {
            if (dropNode == null || dropNode.Tag is ProjectFolder || !projViewer.SelectedNodes.Contains(dropNode))
            {
                return;
            }

            ProjectFile file = dropNode.Tag as ProjectFile;
            Debug.Assert(file != null, "file != null");

            FilePath filePath = file.FileFullPath;
            if (File.Exists(filePath))
            {
                AbstractUiAction.RunCommand(new OpenFileAction(filePath));
            }
            else
            {
                if (MessageBox.Show("File no longer exists, would you like to remove from project?",
                    "File Not Found", MessageBoxButtons.YesNo) != DialogResult.Yes)
                {
                    return;
                }

                ProjectFolder folder = file.Folder;
                folder.DeleteFile(file);
                _model.OnNodesChanged(folder);
            }
        }

        private void openWithMenuItem_Click(object sender, EventArgs e)
        {
            string file = ((ProjectFile) projViewer.SelectedNode.Tag).FileFullPath;
            OpenAs(file);
        }

        private void pasteMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void projectViewer_DoubleClick(object sender, EventArgs e)
        {
            TreeNodeAdv dropNode = projViewer.GetNodeAt(PointToClient(new Point(MousePosition.X, MousePosition.Y)));
            OpenNode(dropNode);
        }

        private void projectViewer_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Enter:
                    foreach (TreeNodeAdv node in projViewer.SelectedNodes)
                    {
                        OpenNode(node);
                    }
                    break;
                case Keys.Delete:
                    DeleteNodes(projViewer.SelectedNodes);
                    break;
            }
        }

        private void projViewer_AfterLabelEdit(object sender, LabelEventArgs e)
        {
            if (string.IsNullOrEmpty(e.NewLabel))
            {
                return;
            }

            ProjectFolder folder = e.Subject as ProjectFolder;
            if (folder != null)
            {
                folder.Name = e.NewLabel;
            }
            else
            {
                ProjectFile file = e.Subject as ProjectFile;
                if (file == null)
                {
                    return;
                }

                FilePath dir = file.FileFullPath.GetDirectoryName();
                if (string.IsNullOrEmpty(dir))
                {
                    return;
                }

                FilePath newFile = dir.Combine(e.NewLabel);
                File.Move(file.FileFullPath, newFile);
                file.FileFullPath = newFile;
            }
        }

        private void projViewer_DragDrop(object sender, DragEventArgs e)
        {
            projViewer.BeginUpdate();

            var nodes = (TreeNodeAdv[])e.Data.GetData(typeof(TreeNodeAdv[]));
            var dropNode = projViewer.DropPosition.Node;
            ProjectFolder nodeFolder = dropNode.Tag as ProjectFolder ?? dropNode.Parent.Tag as ProjectFolder;
            if (nodeFolder == null)
            {
                return;
            }

            foreach (TreeNodeAdv n in nodes)
            {
                ProjectFolder folder = n.Tag as ProjectFolder;
                if (folder == _projectService.Project.MainFolder)
                {
                    continue;
                }

                if (folder != null)
                {
                    folder.Parent.DeleteFolder(folder);
                    nodeFolder.AddFolder(folder);
                }
                else
                {
                    ProjectFile file = n.Tag as ProjectFile;
                    if (file == null)
                    {
                        continue;
                    }

                    file.Remove();
                    nodeFolder.AddFile(file);
                }
            }

            dropNode.Expand();

            BuildProjTree();
            projViewer.EndUpdate();
        }

        private static bool CheckNodeParent(TreeNodeAdv parent, TreeNodeAdv node)
        {
            while (parent != null)
            {
                if (node == parent)
                {
                    return false;
                }

                parent = parent.Parent;
            }
            return true;
        }

        private void projViewer_DragOver(object sender, DragEventArgs e)
        {
            if (!e.Data.GetDataPresent(typeof (TreeNodeAdv[])) || projViewer.DropPosition.Node == null)
            {
                return;
            }
            
            var nodes = e.Data.GetData(typeof(TreeNodeAdv[])) as TreeNodeAdv[];
            TreeNodeAdv parent = projViewer.DropPosition.Node;
            if (projViewer.DropPosition.Position != NodePosition.Inside)
            {
                parent = parent.Parent;
            }

            if (nodes != null && nodes.Any(node => !CheckNodeParent(parent, node)))
            {
                e.Effect = DragDropEffects.None;
                return;
            }

            e.Effect = e.AllowedEffect;
        }

        private void projViewer_ItemDrag(object sender, ItemDragEventArgs e)
        {
            projViewer.DoDragDropSelectedNodes(DragDropEffects.Move);
        }

        private void projViewer_NodeMouseClick(object sender, TreeNodeAdvMouseEventArgs e)
        {
            if (e.Button != MouseButtons.Right)
            {
                return;
            }

            if (e.Node.Tag is ProjectFolder)
            {
                folderContextMenu.Show(projViewer, projViewer.PointToClient(MousePosition));
            }
            else
            {
                fileContextMenu.Show(projViewer, projViewer.PointToClient(MousePosition));
            }
        }

        private static void RenameNode(TreeNodeAdv node)
        {
            var textBox = node.Tree.NodeControls.OfType<NodeTextBox>().Single();
            textBox.BeginEdit();
        }

        private void renFMenuItem_Click(object sender, EventArgs e)
        {
            RenameNode(projViewer.SelectedNode);
        }

        private void renMenuItem_Click(object sender, EventArgs e)
        {
            RenameNode(projViewer.SelectedNode);
        }
    }

    public class NodeSorter : IComparer
    {
        // Compare the length of the strings, or the strings
        // themselves, if they are the same length.
        public int Compare(object x, object y)
        {
            ProjectFolder folderX = x as ProjectFolder;
            ProjectFolder folderY = y as ProjectFolder;
            ProjectFile fileX = x as ProjectFile;
            ProjectFile fileY = y as ProjectFile;

            // Compare the length of the strings, returning the difference.
            if (folderX == null)
            {
                if (fileX != null && fileY != null)
                {
                    return String.CompareOrdinal(fileX.Name, fileY.Name);
                }

                return 0;
            }

            if (folderY != null)
            {
                // If they are the same length, call Compare.
                return String.CompareOrdinal(folderX.Name, folderY.Name);
            }

            return -1;
        }
    }

    internal class ProjectTreeModel : ITreeModel
    {
        private readonly ProjectFolder _rootFolder;

        public ProjectTreeModel(ProjectFolder rootFolder)
        {
            _rootFolder = rootFolder;
        }

        public IEnumerable GetChildren(TreePath treePath)
        {
            var items = new List<object>();
            if (treePath.IsEmpty())
            {
                items.Add(_rootFolder);
                return items;
            }

            ProjectFolder parent = treePath.LastNode as ProjectFolder;
            if (parent == null)
            {
                return items;
            }

            items.AddRange(parent.Folders);
            items.AddRange(parent.Files);
            return items;
        }

        private static TreePath GetPath(ProjectFolder item)
        {
            if (item == null)
            {
                return TreePath.Empty;
            }

            var stack = new Stack<object>();
            while (item != null)
            {
                stack.Push(item);
                item = item.Parent;
            }

            return new TreePath(stack.ToArray());
        }

        public bool IsLeaf(TreePath treePath)
        {
            return treePath.LastNode is ProjectFile;
        }

        public event EventHandler<TreeModelEventArgs> NodesChanged;
        internal void OnNodesChanged(ProjectFolder item)
        {
            if (NodesChanged == null)
            {
                return;
            }

            TreePath path = item != null ? GetPath(item.Parent) : GetPath(null);
            NodesChanged(this, new TreeModelEventArgs(path, new object[] { item }));
        }

        public event EventHandler<TreeModelEventArgs> NodesInserted;
        public event EventHandler<TreeModelEventArgs> NodesRemoved;
        public event EventHandler<TreePathEventArgs> StructureChanged;
        public void OnStructureChanged()
        {
            if (StructureChanged != null)
                StructureChanged(this, new TreePathEventArgs());
        }
    }
}