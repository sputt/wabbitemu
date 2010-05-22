using System;
using System.ComponentModel;
using System.IO;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Docking;
using System.Runtime.InteropServices;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class DirectoryViewer : ToolWindow
    {
        public DirectoryViewer()
        {
            InitializeComponent();
        }

        // Code For OpenWithDialog Box
        [DllImport("shell32.dll", SetLastError = true)]
        extern public static bool ShellExecuteEx(ref ShellExecuteInfo lpExecInfo);
        public const uint SW_NORMAL = 1;
        [Serializable]
        public struct ShellExecuteInfo
        {
            public int Size;
            public uint Mask;
            public IntPtr hwnd;
            public string Verb;
            public string File;
            public string Parameters;
            public string Directory;
            public uint Show;
            public IntPtr InstApp;
            public IntPtr IDList;
            public string Class;
            public IntPtr hkeyClass;
            public uint HotKey;
            public IntPtr Icon;
            public IntPtr Monitor;
        }
        static void OpenAs(string file)
        {
            ShellExecuteInfo sei = new ShellExecuteInfo();
            sei.Size = Marshal.SizeOf(sei);
            sei.Verb = "openas";
            sei.File = file;
            sei.Show = SW_NORMAL;
            if (!ShellExecuteEx(ref sei))
                throw new Win32Exception();
        }

        /// <summary>
        /// Use the projectViewer tree to display the contents of the project directory.
        /// </summary>
#pragma warning disable 1574
        /// <param name="projectFile">Location of the .wcodeproj file</param>
#pragma warning restore 1574
        public void buildDirectoryTree(string[] files)
        {
            dirViewer.Nodes.Clear();
            TreeNode projectNode = new TreeNode
                                       {
                                           Name = "mainNode",
                                           Text =
                                               Path.GetDirectoryName(ProjectService.ProjectFile).Substring(
                                               Path.GetDirectoryName(ProjectService.ProjectFile).LastIndexOf('\\') + 1,
                                               Path.GetDirectoryName(ProjectService.ProjectFile).Length -
                                               Path.GetDirectoryName(ProjectService.ProjectFile).LastIndexOf('\\') - 1)
                                       };
            dirViewer.Nodes.Add(projectNode);
            dirViewer.TopNode = projectNode;
            findAllFilesDirectoryTree(files, projectNode);
            dirViewer.TopNode.Expand();
        }

        public static void findFoldersProjectTree(string[] directories, TreeNode project)
        {
            foreach (string directory in directories)
            {
                TreeNode subFolder = new TreeNode
                {
                    ImageIndex = 2,
                    SelectedImageIndex = 3,
                    Text = directory.Remove(0, directory.LastIndexOf('\\') + 1),
                    Tag = "Folder"
                };
                project.Nodes.Add(subFolder);
                findFoldersProjectTree(Directory.GetDirectories(directory), subFolder);
                string[] files = Directory.GetFiles(directory);
                findAllFilesDirectoryTree(files, subFolder);
            }
        }

        public static void findAllFilesDirectoryTree(string[] files, TreeNode subFolder)
        {
            foreach (String file in files)
                if (file.ToLower().EndsWith(".asm") || file.ToLower().EndsWith(".z80") ||
                    file.ToLower().EndsWith(".inc") || file.ToLower().EndsWith(".bmp"))
                {
                    TreeNode fileName = new TreeNode
                    {
                        Text = file.Remove(0, file.LastIndexOf('\\') + 1),
                        ImageIndex = 4,
                        SelectedImageIndex = 5,
                        Tag = "File"
                    };
                    subFolder.Nodes.Add(fileName);
                }
        }

        private void DirectoryViewer_VisibleChanged(object sender, EventArgs e)
        {
            if (ParentForm == null)
                return;
            Settings.Default.directoryViewer = DockHandler.DockState == DockState.Hidden ? false : true;
            if (ParentForm.GetType() != typeof(FloatWindow))
                ((MainFormRedone)(ParentForm)).UpdateChecks();
            else
                ((MainFormRedone)(ParentForm.Owner)).UpdateChecks();
        }

        private void directoryViewer_DoubleClick(object sender, EventArgs e)
        {
            string projectLoc = ProjectService.ProjectDirectory;
            if (dirViewer.SelectedNode == null || dirViewer.SelectedNode.Tag.ToString() == "Folder") return;
            string fileName = projectLoc.Substring(0, projectLoc.LastIndexOf('\\')) + '\\' + dirViewer.SelectedNode.FullPath;
            if (ParentForm == null)
                return;
			newEditor doc = DocumentService.CreateNewDocument();
            doc.TabText = Path.GetFileName(fileName);
            doc.Text = Path.GetFileName(fileName);
            doc.ToolTipText = fileName;
            doc.Show(((MainFormRedone)ParentForm).dockPanel);
            doc.OpenFile(fileName);
        }

        private void directoryViewer_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            e.Node.ImageIndex = 0;
            e.Node.SelectedImageIndex = 1;
        }

        private void directoryViewer_BeforeCollapse(object sender, TreeViewCancelEventArgs e)
        {
            e.Node.ImageIndex = 2;
            e.Node.SelectedImageIndex = 3;
        }

        private void directoryViewer_ItemDrag(object sender, ItemDragEventArgs e)
        {
            DoDragDrop(e.Item, DragDropEffects.Move);
        }

        private void dirViewer_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            dirViewer.SelectedNode = e.Node;
            if (e.Button == MouseButtons.Right)
            {
                if (e.Node.Text.EndsWith(".asm") || e.Node.Text.EndsWith(".inc") || e.Node.Text.EndsWith(".z80") || e.Node.Text.EndsWith(".bmp"))
                    fileContextMenu.Show(dirViewer, dirViewer.PointToClient(MousePosition));
                if (e.Node.ImageIndex < 4)
                    folderContextMenu.Show(dirViewer, dirViewer.PointToClient(MousePosition));
            }
        }

        private void openMenuItem_Click(object sender, EventArgs e)
        {
            MainFormRedone parent;
            if (ParentForm == null)
                return;
            if (ParentForm.GetType() != typeof(FloatWindow))
                parent = ((MainFormRedone)ParentForm);
            else
                parent = ((MainFormRedone)(ParentForm.Owner));
            string projectLoc = ProjectService.ProjectDirectory;
            string fileName = projectLoc.Substring(0, projectLoc.LastIndexOf('\\')) + '\\' + dirViewer.SelectedNode.FullPath;
            newEditor doc = DocumentService.CreateNewDocument();
            doc.TabText = Path.GetFileName(fileName);
            doc.Text = Path.GetFileName(fileName);
            doc.ToolTipText = fileName;
            doc.Show(parent.dockPanel);
            doc.OpenFile(fileName);
        }

        private void openWithMenuItem_Click(object sender, EventArgs e)
        {
            OpenAs(ProjectService.ProjectDirectory.Substring(0, ProjectService.ProjectDirectory.LastIndexOf('\\')) + '\\' + dirViewer.SelectedNode.FullPath);
        }

        private void renMenuItem_Click(object sender, EventArgs e)
        {
            dirViewer.SelectedNode.BeginEdit();
        }

        private void delMenuItem_Click(object sender, EventArgs e)
        {
            string projectLoc = ProjectService.ProjectDirectory;
            string fileName = projectLoc.Substring(0, projectLoc.LastIndexOf('\\') + 1) + dirViewer.SelectedNode.FullPath;
            if (MessageBox.Show("Would you like to delete " + dirViewer.SelectedNode.Text + "?", "Delete File?", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                File.Delete(fileName);
                buildDirectoryTree(Directory.GetDirectories(projectLoc));
            }
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

        private void dirViewer_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            if (e.Label == null)
                return;
            if (e.Label == "")
            {
                MessageBox.Show("You must enter a name!");
                e.CancelEdit = true;
                return;
            }
            string projectLoc = ProjectService.ProjectDirectory;
            string oldFileName = projectLoc.Substring(0, projectLoc.LastIndexOf('\\') + 1) + e.Node.FullPath;
            string newFileName = projectLoc.Substring(0, projectLoc.LastIndexOf('\\') + 1) + e.Node.FullPath.Substring(0, e.Node.FullPath.LastIndexOf('\\') + 1) + e.Label;
            if (e.Node.Tag.ToString() == "Folder")
                Directory.Move(oldFileName, newFileName);
            else
                File.Move(oldFileName, newFileName);
        }

        private void openIEMenuItem_Click(object sender, EventArgs e)
        {
            string folderName = ProjectService.ProjectDirectory.Substring(0, ProjectService.ProjectDirectory.LastIndexOf('\\') + 1) + dirViewer.SelectedNode.FullPath;
            System.Diagnostics.Process.Start(folderName);
        }

        private void renFMenuItem_Click(object sender, EventArgs e)
        {
            dirViewer.SelectedNode.BeginEdit();
        }

        private void delFMenuItem_Click(object sender, EventArgs e)
        {
            string projectLoc = ProjectService.ProjectDirectory;
            string folderName = projectLoc.Substring(0, projectLoc.LastIndexOf('\\') + 1) + dirViewer.SelectedNode.FullPath;
            if (MessageBox.Show("Would you like to delete " + dirViewer.SelectedNode.Text + "?", "Delete Folder?",
                                MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes) return;
            Directory.Delete(folderName, true);
            buildDirectoryTree(Directory.GetDirectories(projectLoc));
        }

        private void cutFMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void copyFMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void pasteFMenuItem_Click(object sender, EventArgs e)
        {

        }
    }
}