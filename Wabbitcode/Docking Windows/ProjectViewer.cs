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

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class ProjectViewer : ToolWindow
    {
        //private readonly ProjectClass project;
        public ProjectViewer()
        {
            InitializeComponent();
        }

        public void updateNodeTag(TreeNode parent, string fileName, string tag)
        {
            foreach (TreeNode node in parent.Nodes)
            {
                if (node.Tag.ToString() == "Folder")
                    updateNodeTag(node, fileName, tag);
                else if (node.Tag.ToString().Split('|')[1] == fileName)
                    node.Tag = tag;
            }
        }

        public void buildXMLFile(ref XmlDocument doc)
        {
            XmlElement parentXml = doc.CreateElement("filesystem");
            XmlAttribute name = doc.CreateAttribute("name");
            name.Value = projViewer.Nodes[0].Text;
            parentXml.Attributes.Append(name);
            getXMLFolders(projViewer.Nodes[0], ref parentXml, ref doc);
            if (doc.DocumentElement == null)
                throw new NullReferenceException();
            doc.DocumentElement.RemoveChild(doc.DocumentElement.ChildNodes[0]);
            doc.DocumentElement.InsertBefore(parentXml, doc.DocumentElement.ChildNodes[0]);
        }

        private static void getXMLFolders(TreeNode parent, ref XmlElement parentXml, ref XmlDocument doc)
        {
            foreach (TreeNode node in parent.Nodes)
                if (node.Tag.ToString() == "Folder")
                {
                    XmlElement element = doc.CreateElement("dir");
                    XmlAttribute name = doc.CreateAttribute("name");
                    name.Value = node.Text + "\\";
                    element.Attributes.Append(name);
                    parentXml.AppendChild(element);
                    getXMLFolders(node, ref element, ref doc);
                }
                else
                {
                    XmlElement element = doc.CreateElement("file");
                    element.InnerText = node.Text;
                    XmlAttribute path = doc.CreateAttribute("Path");
                    XmlAttribute foldings = doc.CreateAttribute("Foldings");
                    string[] attributes = node.Tag.ToString().Split('|');
                    path.Value = attributes[1];
                    if (attributes.Length > 2)
                    {
                        foldings.Value = attributes[2];
                        element.Attributes.Append(foldings);
                    }
                    element.Attributes.Append(path);
                    parentXml.AppendChild(element);
                }
        }

        public void buildProjTree(string projectFile)
        {
            projViewer.Nodes.Clear();
            projViewer.TreeViewNodeSorter = new NodeSorter();
            XmlDocument doc = new XmlDocument();
            doc.Load(projectFile);
            TreeNode parent = new TreeNode
                                  {
                                      Tag = "Folder",
                                      Text = doc.ChildNodes[1].Attributes[0].InnerText.Substring(0, doc.ChildNodes[1].
                                                    Attributes[0].InnerText.LastIndexOf('.'))
                                  };
            projViewer.Nodes.Add(parent);
            Settings.Default.projectViewer = true;
            Visible = true;
            foreach (XmlNode node in doc.ChildNodes[1].ChildNodes)
            {
                if (node.NodeType != XmlNodeType.Element || node.Name != "filesystem") 
                    continue;
                getNodeFolders(node, parent);
                getNodeFiles(node, parent);
            }
			parent.Expand();
            projViewer.Sort();
        }

        private static void getNodeFolders(XmlNode dir, TreeNode nodeToAddTo)
        {
            foreach (XmlNode child in dir.ChildNodes)
                if (child.Name == "dir")
                {
                    TreeNode newNode = new TreeNode
                                      {
                                          ImageIndex = 2,
                                          SelectedImageIndex = 3,
                                          Text = child.Attributes[0].Value.Remove(child.Attributes[0].Value.Length - 1),
                                          Tag = "Folder"
                                      };
                    nodeToAddTo.Nodes.Add(newNode);
                    getNodeFolders(child, newNode);
                    getNodeFiles(child, newNode);
                }
        }

        private static void getNodeFiles(XmlNode dir, TreeNode nodeToAddTo)
        {
            foreach (XmlNode child in dir.ChildNodes)
                if (child.Name == "file")
                {
                    TreeNode newNode = new TreeNode
                                      {
                                          Text = child.InnerText,
                                          ImageIndex = 4,
                                          SelectedImageIndex = 5,
                                          Tag = child.Attributes["Foldings"] != null ?
                                              "File|" + child.Attributes["Path"].Value + "|" +
                                              child.Attributes["Foldings"].Value : "File|" + child.Attributes["Path"].Value
                                      };
                    nodeToAddTo.Nodes.Add(newNode);
                }
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

        private void ProjectViewer_VisibleChanged(object sender, EventArgs e)
        {
            Settings.Default.projectViewer = DockHandler.DockState == DockState.Hidden ? false : true;
			DockingService.MainForm.UpdateChecks();
        }

        private void projectViewer_DoubleClick(object sender, EventArgs e)
        {
            TreeNode dropNode = projViewer.GetNodeAt(PointToClient(new Point(MousePosition.X, MousePosition.Y)));
            if (dropNode == null || dropNode.Tag.ToString() == "Folder" || !projViewer.SelectedNodes.Contains(dropNode)) 
                return;
            string[] attributes = dropNode.Tag.ToString().Split('|');
            string file = Path.Combine(Path.GetDirectoryName(ProjectService.ProjectFile),
                                       attributes[1].Substring(1));
            if (File.Exists(file))
            {
                newEditor doc = DocumentService.GotoFile(file);
                if (attributes.Length > 2)
                    doc.editorBox.Document.FoldingManager.DeserializeFromString(attributes[2]);
            }
            else if (MessageBox.Show("File no longer exists, would you like to remove from project?", "File Not Found", MessageBoxButtons.YesNo) == DialogResult.Yes)
                dropNode.Remove();
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
            if (e.Node.Text.EndsWith(".asm") || e.Node.Text.EndsWith(".inc") || e.Node.Text.EndsWith(".z80") || e.Node.Text.EndsWith(".bmp"))
                fileContextMenu.Show(projViewer, projViewer.PointToClient(MousePosition));
            if (e.Node.ImageIndex < 4)
                folderContextMenu.Show(projViewer, projViewer.PointToClient(MousePosition));
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
            projViewer.SelectedNodes[0].BeginEdit();
        }

        private void renFMenuItem_Click(object sender, EventArgs e)
        {
            projViewer.SelectedNodes[0].BeginEdit();
        }

        private void openMenuItem_Click(object sender, EventArgs e)
        {
            foreach (TreeNode node in projViewer.SelectedNodes)
            {
                string[] attributes = node.Tag.ToString().Split('|');
				string file = attributes[1];
				if (file.Length > 0 && file[0] == '\\')
					file = file.Remove(0, 1);
				if (!Path.IsPathRooted(file))
					file = Path.Combine(ProjectService.ProjectDirectory, file);
                newEditor doc = DocumentService.GotoFile(file);
                if (!string.IsNullOrEmpty(attributes[2]))
                    doc.editorBox.Document.FoldingManager.DeserializeFromString(attributes[2]);
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
            newNode.BeginEdit();
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
            while (newNode.Tag.ToString() != "Folder")
                newNode = newNode.Parent;
            foreach (TreeNode original in projViewer.SelectedNodes)
            {
                if (newNode == original)
                    break;
                original.Remove();
                newNode.Nodes.Add(original);
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
                node.Remove();
            }
            ProjectService.Project.saveProject();
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
                if (tx.Tag.ToString() == "Folder" && ty.Tag.ToString() == "Folder")
                    return string.Compare(tx.Text, ty.Text);
                if (tx.Tag.ToString() == "Folder")
                    return -1;
                return ty.Tag.ToString() == "Folder" ? 1 : string.Compare(tx.Text, ty.Text);
                // If they are the same length, call Compare.
            }
        }   

        private void delFMenuItem_Click(object sender, EventArgs e)
        {
			if (MessageBox.Show("Are you sure you want to remove this folder from the project?", "Delete Folder", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation) == DialogResult.No)
                return;
            foreach (TreeNode original in projViewer.SelectedNodes)
                original.Remove();
			ProjectService.Project.saveProject();
            projViewer.Sort();

        }

        private void existingFileMenuItem_Click(object sender, EventArgs e)
        {
            DockingService.MainForm.existingFileMenuItem_Click(sender, e);
        }
    }
}