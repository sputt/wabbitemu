using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Project;
using System;
using System.Collections;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;


namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class ProjectViewer : ToolWindow
	{
		#region Private Members

		private readonly IDockingService _dockingService;
		private readonly IDocumentService _documentService;
		private readonly IProjectService _projectService;

		#endregion

		public ProjectViewer(IDockingService dockingService, IDocumentService documentService,
			IProjectService projectService)
			: base(dockingService)
		{
			InitializeComponent();

			_dockingService = dockingService;
			_documentService = documentService;
			_projectService = projectService;
		}

		private void AddFile(ProjectFile file, TreeNode parent)
		{
			TreeNode nodeFile = new TreeNode
			{
				Tag = file,
				Text = Path.GetFileName(file.FileFullPath),
				ImageIndex = 4,
				SelectedImageIndex = 5,
			};

			if (parent == null)
			{
				projViewer.Nodes.Add(nodeFile);
			}
			else
			{
				parent.Nodes.Add(nodeFile);
			}
		}

		public void BuildProjTree()
		{
			projViewer.Nodes.Clear();
			projViewer.TreeViewNodeSorter = new NodeSorter();
			ProjectFolder folder = _projectService.Project.MainFolder;
			if (folder == null)
			{
				return;
			}
			RecurseAddNodes(folder, null);
			if (projViewer.Nodes.Count > 0)
			{
				projViewer.Nodes[0].Expand();
			}
			projViewer.Sort();
		}

		private void DeleteNode(TreeNode node)
		{
			var folder = node.Tag as ProjectFolder;
			if (folder != null)
			{
				folder.Remove();
				node.Remove();
			}
			else
			{
				string fileName = Path.Combine(Path.GetDirectoryName(_projectService.Project.ProjectDirectory), node.FullPath);
				File.Delete(fileName);
				node.Remove();
			}
		}

		private void DeleteNodes(NodesCollection nodes)
		{
			if (nodes.Count < 1)
			{
				return;
			}
			string message = nodes.Count > 1 ? "Would you like to delete the selected files permanently?" :
							 "Delete '" + nodes[0].Text + "' permanently?";

			DialogResult results = MessageBox.Show(message, "Delete Contents", MessageBoxButtons.YesNo, MessageBoxIcon.Question);

			if (results != DialogResult.Yes)
			{
				return;
			}

			foreach (TreeNode node in nodes)
			{
				DeleteNode(node);
			}

			projViewer.SelectedNodes.Clear();
			projViewer.Sort();
		}

		internal void AddExistingFile(string file)
		{
			try
			{
				TreeNode parent = projViewer.SelectedNode;
				if (parent == null)
				{
					parent = projViewer.Nodes[0];
				}
				else if (parent.Tag is ProjectFile)
				{
					parent = parent.Parent;
				}

				ProjectFile fileAdded = _projectService.AddFile((ProjectFolder)parent.Tag, file);
				AddFile(fileAdded, parent);
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error adding file", ex);
			}
		}

		internal void AddNewFile(string fileName)
		{
			TreeNode parent = projViewer.SelectedNode;
			if (parent == null)
			{
				parent = projViewer.Nodes[0];
			}
			else if (parent.Tag is ProjectFile)
			{
				parent = parent.Parent;
			}

			if (parent == null)
			{
				parent = projViewer.Nodes[0];
			}

			string file = Path.Combine(_projectService.Project.ProjectDirectory, fileName);
			StreamWriter writer = File.CreateText(file);
			writer.Close();
			ProjectFile fileAdded = _projectService.AddFile((ProjectFolder)parent.Tag, file);
			AddFile(fileAdded, parent);
			_documentService.OpenDocument(file);
		}

		internal void CloseProject()
		{
			projViewer.Nodes.Clear();
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
			{
				projViewer.Nodes.Add(nodeFolder);
			}
			else
			{
				parent.Nodes.Add(nodeFolder);
			}
			return nodeFolder;
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
			DeleteNodes(projViewer.SelectedNodes);
		}

		private void excFromProj_Click(object sender, EventArgs e)
		{
			foreach (TreeNode node in projViewer.SelectedNodes)
			{
				if (node.Tag is ProjectFolder)
				{
					continue;
				}

				ProjectFile file = (ProjectFile)node.Tag;
				_projectService.DeleteFile(file.Folder, file);
				node.Remove();
			}

			// ProjectService.Project.saveProject();
		}

		private void existingFileMenuItem_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog
			{
				CheckFileExists = true,
				DefaultExt = "*.asm",
				Filter = "All Know File Types | *.asm; *.z80; *.inc; |Assembly Files (*.asm)|*.asm|*.z80" +
						 " Assembly Files (*.z80)|*.z80|Include Files (*.inc)|*.inc|All Files(*.*)|*.*",
				FilterIndex = 0,
				Multiselect = true,
				RestoreDirectory = true,
				Title = "Add Existing File",
			};
			DialogResult result = openFileDialog.ShowDialog();
			if (result != DialogResult.OK)
			{
				return;
			}

			foreach (string file in openFileDialog.FileNames)
			{
				AddExistingFile(file);
			}
		}

		private ProjectFile GetFileFromPath(string path)
		{
			ProjectFile fileFound = null;
			ProjectFolder current = _projectService.Project.MainFolder;
			string[] folders = path.Split('\\');
			foreach (string folder in folders)
			{
				ProjectFolder newFolder = current.FindFolder(folder);
				if (newFolder == null)
				{
					fileFound = current.FindFile(folder);
				}
				else
				{
					current = newFolder;
				}
			}

			return fileFound;
		}

		private void newFileContextItem_Click(object sender, EventArgs e)
		{
			RenameForm newNameForm = new RenameForm
			{
				Text = "New File"
			};
			if (newNameForm.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			string name = newNameForm.NewText;
			AddNewFile(name);
		}

		private void newFolderContextItem_Click(object sender, EventArgs e)
		{
			TreeNode parent = projViewer.SelectedNode ?? projViewer.Nodes[0];

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

			string name = newNameForm.NewText;
			ProjectFolder folder = new ProjectFolder(_projectService.Project, name);
			AddFolder(folder, parent);
			((ProjectFolder)parent.Tag).AddFolder(folder);
			projViewer.Sort();
		}

		private void openExplorerMenuItem_Click(object sender, EventArgs e)
		{
			Process explorer = new Process
			{
				StartInfo =
				{
					FileName = Path.Combine(Path.GetDirectoryName(_projectService.Project.ProjectDirectory),
					projViewer.SelectedNode.FullPath)
				}
			};
			explorer.Start();
		}

		private void openMenuItem_Click(object sender, EventArgs e)
		{
			OpenNode(projViewer.SelectedNode);
			foreach (TreeNode node in projViewer.SelectedNodes)
			{
				OpenNode(node);
			}
		}

		private void OpenNode(TreeNode dropNode)
		{
			if (dropNode == null || dropNode.Tag.GetType() == typeof(ProjectFolder) || !projViewer.SelectedNodes.Contains(dropNode))
			{
				return;
			}

			ProjectFile file = GetFileFromPath(dropNode.FullPath);
			if (file == null)
			{
				return;
			}

			string filePath = file.FileFullPath;
			if (File.Exists(filePath))
			{
				_documentService.GotoFile(filePath);
			}
			else
			{
				if (MessageBox.Show("File no longer exists, would you like to remove from project?",
					"File Not Found", MessageBoxButtons.YesNo) != DialogResult.Yes)
				{
					return;
				}
				dropNode.Remove();
				projViewer.SelectedNodes.Remove(dropNode);
			}
		}

		private void openWithMenuItem_Click(object sender, EventArgs e)
		{
			string file = ((ProjectFile)projViewer.SelectedNode.Tag).FileFullPath;
			OpenAs(file);
		}

		private void pasteMenuItem_Click(object sender, EventArgs e)
		{
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

		private void projectViewer_DoubleClick(object sender, EventArgs e)
		{
			TreeNode dropNode = projViewer.GetNodeAt(PointToClient(new Point(MousePosition.X, MousePosition.Y)));
			OpenNode(dropNode);
		}

		private void projectViewer_KeyDown(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					foreach (TreeNode node in projViewer.SelectedNodes)
					{
						OpenNode(node);
					}
					break;
				case Keys.Delete:
					DeleteNodes(projViewer.SelectedNodes);
					break;
			}
		}

		private void projViewer_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
		{
			if (!string.IsNullOrEmpty(e.Label))
			{
				if (e.Node.Tag is ProjectFolder)
				{
					ProjectFolder folder = e.Node.Tag as ProjectFolder;
					folder.Name = e.Label;
				}
				else
				{
					ProjectFile file = e.Node.Tag as ProjectFile;
					if (file != null)
					{
						string newFile = Path.Combine(Path.GetDirectoryName(file.FileFullPath), e.Label);
						File.Move(file.FileFullPath, newFile);
						file.FileFullPath = newFile;
					}
				}

				return;
			}

			MessageBox.Show("You must enter a name!");
			e.CancelEdit = true;
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

		private void projViewer_DragDrop(object sender, DragEventArgs e)
		{
			Point cursor = projViewer.PointToClient(new Point(e.X, e.Y));
			TreeNode newNode = projViewer.GetNodeAt(cursor);
			if (newNode == null)
			{
				return;
			}
			while (!(newNode.Tag is ProjectFolder))
			{
				newNode = newNode.Parent;
			}

			foreach (TreeNode original in projViewer.SelectedNodes)
			{
				if (newNode == original)
				{
					break;
				}

				if (original.Tag is ProjectFolder)
				{
					ProjectFolder folder = original.Tag as ProjectFolder;
					ProjectFolder newParent = newNode.Tag as ProjectFolder;
					if (folder == newParent)
					{
						continue;
					}

					folder.Remove();
					if (newParent != null)
					{
						newParent.AddFolder(folder);
					}
					original.Remove();
					newNode.Nodes.Add(original);
				}
				else
				{
					var projectFile = original.Tag as ProjectFile;
					if (projectFile != null)
					{
						ProjectFile file = projectFile;
						ProjectFolder newParent = (ProjectFolder)newNode.Tag;
						if (file.Folder == newParent)
						{
							continue;
						}

						file.Remove();
						newParent.AddFile(file);
						original.Remove();
						newNode.Nodes.Add(original);
					}
				}

				newNode.Expand();
			}

			projViewer.Sort();
		}

		private void projViewer_DragOver(object sender, DragEventArgs e)
		{
		}

		private void projViewer_ItemDrag(object sender, ItemDragEventArgs e)
		{
		}

		private void projViewer_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
		{
			if (e.Button != MouseButtons.Right)
			{
				return;
			}
			openExplorerMenuItem.Enabled = Directory.Exists(Path.Combine(Path.GetDirectoryName(_projectService.Project.ProjectDirectory), e.Node.FullPath));
			if (e.Node.Tag is ProjectFolder)
			{
				folderContextMenu.Show(projViewer, projViewer.PointToClient(MousePosition));
			}
			else
			{
				fileContextMenu.Show(projViewer, projViewer.PointToClient(MousePosition));
			}
		}

		private void RecurseAddNodes(ProjectFolder folder, TreeNode parentNode)
		{
			TreeNode nodeAdded = AddFolder(folder, parentNode);
			foreach (ProjectFolder subFolder in folder.Folders)
			{
				RecurseAddNodes(subFolder, nodeAdded);
			}
			foreach (ProjectFile file in folder.Files)
			{
				AddFile(file, nodeAdded);
			}
		}

		private void RenameFile(TreeNode node)
		{
			string oldName = node.Text;
			RenameForm renameForm = new RenameForm();
			DialogResult result = renameForm.ShowDialog();
			if (result != DialogResult.OK)
			{
				return;
			}
			string newName = renameForm.NewText;
			if (oldName == newName)
			{
				return;
			}
			ProjectFile file = (ProjectFile)node.Tag;
			string newFileName = Path.Combine(Path.GetDirectoryName(file.FileFullPath), newName);
			File.Move(file.FileFullPath, newFileName);
			foreach (NewEditor editor in _dockingService.Documents
				.Where(editor => string.Equals(editor.FileName, file.FileFullPath, StringComparison.OrdinalIgnoreCase)))
			{
				editor.FileName = newFileName;
			}
			file.FileFullPath = newFileName;
			node.Text = newName;
			projViewer.Sort();
		}

		private void RenameFolder(TreeNode node)
		{
			string oldName = node.Text;
			RenameForm renameForm = new RenameForm
			{
				Text = "Rename Folder"
			};
			DialogResult result = renameForm.ShowDialog();
			if (result != DialogResult.OK)
			{
				return;
			}

			string newName = renameForm.NewText;
			if (oldName == newName)
			{
				return;
			}

			ProjectFolder folder = (ProjectFolder)node.Tag;
			folder.Name = newName;
			node.Text = newName;
			projViewer.Sort();
		}

		private void renFMenuItem_Click(object sender, EventArgs e)
		{
			RenameFolder(projViewer.SelectedNode);
		}

		private void renMenuItem_Click(object sender, EventArgs e)
		{
			RenameFile(projViewer.SelectedNode);
		}
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
			{
				return 0;
			}

			// Compare the length of the strings, returning the difference.
			if (tx.Tag is ProjectFolder && ty.Tag is ProjectFolder)
			{
				return string.Compare(tx.Text, ty.Text);
			}
			if (tx.Tag is ProjectFolder)
			{
				return -1;
			}
			return ty.Tag is ProjectFolder ? 1 : string.Compare(tx.Text, ty.Text);

			// If they are the same length, call Compare.
		}
	}
}