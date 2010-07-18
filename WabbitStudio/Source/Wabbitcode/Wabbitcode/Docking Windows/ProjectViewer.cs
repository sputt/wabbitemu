using System;
using System.Collections;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
using Revsoft.Docking;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using System.Xml;
using System.Diagnostics;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class ProjectViewer : ToolWindow
    {
        //private readonly ProjectClass project;
        public ProjectViewer()
        {
            InitializeComponent();
        }

        public void BuildProjTree()
        {
            projViewer.Nodes.Clear();
            projViewer.TreeViewNodeSorter = new NodeSorter();
			ProjectFolder folder = ProjectService.MainFolder;
			RecurseAddNodes(folder, null);
			if (projViewer.Nodes.Count > 0)
                projViewer.Nodes[0].Expand();
            projViewer.Sort();
        }
		
		private void RecurseAddNodes(ProjectFolder folder, TreeNode parentNode)
		{
			TreeNode nodeAdded = AddFolder(folder, parentNode);
			foreach (ProjectFolder subFolder in folder.Folders)
				RecurseAddNodes(subFolder, nodeAdded);
			foreach (ProjectFile file in folder.Files)
				AddFile(file, nodeAdded);
		}

		private TreeNode AddFolder(ProjectFolder folder, TreeNode parent)
		{
			TreeNode nodeFolder = new TreeNode
			{
				Tag = folder,
				Text = folder.Name,
				ImageIndex = 2,
				SelectedImageIndex = 3,
			};
			if (parent == null)
				projViewer.Nodes.Add(nodeFolder);
			else
				parent.Nodes.Add(nodeFolder);
			return nodeFolder;
		}

		public void AddFile(ProjectFile file, TreeNode parent)
		{
			TreeNode nodeFile = new TreeNode
			{
				Tag = file,
				Text = Path.GetFileName(file.FileFullPath),
				ImageIndex = 4,
				SelectedImageIndex = 5,
			};
			if (parent == null)
				projViewer.Nodes.Add(nodeFile);
			else
				parent.Nodes.Add(nodeFile);
		}
        
        static void OpenAs(string file)
        {
            Process p = new Process();
            ProcessStartInfo pi = new ProcessStartInfo("rundll32.exe")
                                      {
                                          UseShellExecute = false,
                                          RedirectStandardOutput = true,
                                          Arguments = "shell32.dll ,OpenAs_RunDLL " + file
                                      };
            p.StartInfo = pi;
            p.Start();

        }

        private void projectViewer_DoubleClick(object sender, EventArgs e)
        {
            TreeNode dropNode = projViewer.GetNodeAt(PointToClient(new Point(MousePosition.X, MousePosition.Y)));
			OpenNode(dropNode);
        }

		private void OpenNode(TreeNode dropNode)
		{
			if (dropNode == null || dropNode.Tag.GetType() == typeof(ProjectFolder) || !projViewer.SelectedNodes.Contains(dropNode))
				return;
			ProjectFile file = GetFileFromPath(dropNode.FullPath);
            if (file == null)
                return;
			string filePath = file.FileFullPath;
			if (File.Exists(filePath))
				DocumentService.GotoFile(filePath);
			else 
				if (MessageBox.Show("File no longer exists, would you like to remove from project?", "File Not Found", MessageBoxButtons.YesNo) == DialogResult.Yes)
				dropNode.Remove();
		}

		private ProjectFile GetFileFromPath(string path)
		{
			ProjectFile fileFound = null;
			ProjectFolder current = ProjectService.MainFolder;
			string[] folders = path.Split('\\');
			foreach (string folder in folders)
			{
				ProjectFolder newFolder = current.FindFolder(folder);
				if (newFolder == null)
					fileFound = current.FindFile(folder);
				else
					current = newFolder;
			}
			return fileFound;
		}

        private void projectViewer_BeforeCollapse(object sender, TreeViewCancelEventArgs e)
        {
            e.Node.ImageIndex = 2;
            e.Node.SelectedImageIndex = 3;
        }

        private void projectViewer_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            e.Node.ImageIndex = 0;
            e.Node.SelectedImageIndex = 1;
        }

        private void projViewer_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (e.Button != MouseButtons.Right) 
                return;
            if (string.IsNullOrEmpty(Path.GetExtension(e.Node.Text)))
				folderContextMenu.Show(projViewer, projViewer.PointToClient(MousePosition));
            else
                fileContextMenu.Show(projViewer, projViewer.PointToClient(MousePosition));
        }

		private void RenameFile(TreeNode node)
		{
			string oldName = node.Text;
			projViewer.LabelEdit = true;
			node.BeginEdit();
			projViewer.LabelEdit = false;
			string newName = node.Text;
			if (oldName == newName)
				return;
			ProjectFile file = (ProjectFile)node.Tag;
			string newFileName = Path.Combine(Path.GetDirectoryName(file.FileFullPath), newName);
			File.Move(file.FileFullPath, newFileName);
			file.FileFullPath = newFileName;
		}

        private void projViewer_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            if (e.Label != "")
            {
                if (e.Node.Tag.ToString() == "Folder" || e.Label == null)
                    return;
                string[] attributes = e.Node.Tag.ToString().Split('|');
                string file = Path.Combine(ProjectService.ProjectDirectory, attributes[1].Substring(1));
                string newFile = Path.Combine(Path.GetDirectoryName(file), e.Label);
                File.Move(file, newFile);
                e.Node.Tag = attributes[0] + "|" + newFile + "|" + attributes[2];
                return;
            }
            MessageBox.Show("You must enter a name!");
            e.CancelEdit = true;
            return;
            /*foreach (TreeNode node in e.Node.Parent.Nodes)
                if (node.Text == e.Label)
                {
                    if (node.Tag.ToString() == "Folder")
                        MessageBox.Show("Folder already exists!");
                    else
                        MessageBox.Show("File already exists!");
                    e.CancelEdit = true;
                    return;
                }*/
        }

        private void renMenuItem_Click(object sender, EventArgs e)
        {
			RenameFile(projViewer.SelectedNodes[0]);
        }

        private void renFMenuItem_Click(object sender, EventArgs e)
        {
			//RenameFolder(projViewer.SelectedNodes[0]);
            TreeNode node = projViewer.SelectedNodes[0];
            string oldName = node.Text;
            projViewer.LabelEdit = true;
            node.BeginEdit();
            projViewer.LabelEdit = false;
            string newName = node.Text;
            if (oldName == newName)
                return;
            ProjectFolder folder = (ProjectFolder)node.Tag;
            folder.Name = newName;
        }

        private void openMenuItem_Click(object sender, EventArgs e)
        {
            foreach (TreeNode node in projViewer.SelectedNodes)
            {
				OpenNode(node);
				/*
                string[] attributes = node.Tag.ToString().Split('|');
				string file = attributes[1];
				if (file.Length > 0 && file[0] == '\\')
					file = file.Remove(0, 1);
				if (!Path.IsPathRooted(file))
					file = Path.Combine(ProjectService.ProjectDirectory, file);
                newEditor doc = DocumentService.GotoFile(file);
                if (!string.IsNullOrEmpty(attributes[2]))
                    doc.editorBox.Document.FoldingManager.DeserializeFromString(attributes[2]);*/
            }
        }

        private void openWithMenuItem_Click(object sender, EventArgs e)
        {
            string file = Path.Combine(Path.GetDirectoryName(ProjectService.ProjectFile),
                                       projViewer.SelectedNodes[0].Tag.ToString().Split('|')[1]);
            OpenAs(file);
        }

        private void delMenuItem_Click(object sender, EventArgs e)
        {
            string projectLoc = ProjectService.ProjectDirectory;
            DialogResult results;
            if (projViewer.SelectedNodes.Count > 1)
                results = MessageBox.Show("Would you like to delete the selected files?", "Delete Files?",
                                          MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            else
                results = MessageBox.Show("Would you like to delete " + projViewer.SelectedNodes[0].Text + "?",
                                          "Delete File?", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (results != DialogResult.Yes)
                return;
            foreach (TreeNode node in projViewer.SelectedNodes)
            {
                string fileName = projectLoc.Substring(0, projectLoc.LastIndexOf('\\') + 1) + node.FullPath;
                File.Delete(fileName);
                node.Remove();
            }
            projViewer.Sort();
        }

        private void newFolderContextItem_Click(object sender, EventArgs e)
        {
            TreeNode node = projViewer.SelectedNodes[0];
            TreeNode newNode = new TreeNode("New Folder", 2, 3) {Tag = "Folder"};
            node.Nodes.Add(newNode);
            node.Expand();
            projViewer.Sort();
			projViewer.LabelEdit = true;
            newNode.BeginEdit();
			projViewer.LabelEdit = false;
			ProjectFolder folder = new ProjectFolder(ProjectService.Project, newNode.Name);

        }

        private void cutMenuItem_Click(object sender, EventArgs e)
        {
            
        }

        private void copyMenuItem_Click(object sender, EventArgs e)
        {
            
        }

        private void pasteMenuItem_Click(object sender, EventArgs e)
        {

        }
        
        private void projViewer_ItemDrag(object sender, ItemDragEventArgs e)
        {

        }

        private void projViewer_DragDrop(object sender, DragEventArgs e)
        {
            Point cursor = projViewer.PointToClient(new Point(e.X, e.Y));
            TreeNode newNode = projViewer.GetNodeAt(cursor);
			if (newNode == null)
				return;
            while (newNode.Tag.GetType() != typeof(ProjectFolder))
                newNode = newNode.Parent;
            foreach (TreeNode original in projViewer.SelectedNodes)
            {
                if (newNode == original)
                    break;
                if (original.Tag.GetType() == typeof(ProjectFolder))
                {
                    ProjectFolder folder = original.Tag as ProjectFolder;
                    ProjectFolder newParent = newNode.Tag as ProjectFolder;
                    if (folder == newParent)
                        continue;
                    folder.Remove();
                    newParent.AddFolder(folder);
                    original.Remove();
                    newNode.Nodes.Add(original);
                }
                else if (original.Tag.GetType() == typeof(ProjectFile))
                {
                    ProjectFile file = original.Tag as ProjectFile;
                    ProjectFolder newParent = newNode.Tag as ProjectFolder;
                    if (file.Folder == newParent)
                        continue;
                    file.Remove();
                    newParent.AddFile(file);
                    original.Remove();
                    newNode.Nodes.Add(original);
                }
                newNode.Expand();
            }
            projViewer.Sort();
        }
        
        private void projViewer_DragOver(object sender, DragEventArgs e)
        {
            
        }

        private void excFromProj_Click(object sender, EventArgs e)
        {
            foreach (TreeNode node in projViewer.SelectedNodes)
            {
                if (node.Tag.ToString() == "Folder")
                    continue;
                ProjectFile file = (ProjectFile)node.Tag;
                ProjectService.DeleteFile(file.Folder, file);
                node.Remove();
            }
            //ProjectService.Project.saveProject();
        }

        public class NodeSorter : IComparer
        {
            // Compare the length of the strings, or the strings
            // themselves, if they are the same length.
            public int Compare(object x, object y)
            {
                TreeNode tx = x as TreeNode;
                TreeNode ty = y as TreeNode;

                if (tx == null || ty == null)
                    return 0;
                // Compare the length of the strings, returning the difference.
                if (tx.Tag.GetType() == typeof(ProjectFolder) && ty.Tag.GetType() == typeof(ProjectFolder))
                    return string.Compare(tx.Text, ty.Text);
                if (tx.Tag.GetType() == typeof(ProjectFolder))
                    return -1;
                return ty.Tag.GetType() == typeof(ProjectFolder) ? 1 : string.Compare(tx.Text, ty.Text);
                // If they are the same length, call Compare.
            }
        }   

        private void delFMenuItem_Click(object sender, EventArgs e)
        {
			if (MessageBox.Show("Are you sure you want to remove this folder from the project?", "Delete Folder", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation) == DialogResult.No)
                return;
            foreach (TreeNode original in projViewer.SelectedNodes)
                original.Remove();
			//ProjectService.Project.saveProject();
            projViewer.Sort();

        }

        private void existingFileMenuItem_Click(object sender, EventArgs e)
        {
            DockingService.MainForm.existingFileMenuItem_Click(sender, e);
        }
    }
}