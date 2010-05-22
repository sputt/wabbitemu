using System;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using Revsoft.Wabbitcode.Classes;
using System.Collections;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode
{
    public partial class templateForm
    {
        public string projectFile = "";
//        private DockPanel dockPanel;
        public templateForm()
        {
            InitializeComponent();
            if (!Directory.Exists(Path.Combine(Application.UserAppDataPath, "Templates")))
                createTemplates();
            getTemplates();
            calcOptions.SelectedIndex = 0;
            getTemplatesInDir();
        }

        readonly string templatesdir = Path.Combine(Application.UserAppDataPath, "Templates");
        private void getTemplates()
        {
            string[] dirs = Directory.GetDirectories(templatesdir);
            foreach (string dir in dirs)
                calcOptions.Items.Add(Path.GetFileName(dir));
        }

        private void getTemplatesInDir()
        {
            string[] templates = Directory.GetDirectories(Path.Combine(templatesdir, calcOptions.Items[calcOptions.SelectedIndex].ToString()));
            foreach (string template in templates)
                listView1.Items.Add(Path.GetFileName(template));
        }

        private void createTemplates()
        {
            Directory.CreateDirectory(templatesdir);
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-73"));
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-82"));
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-83"));
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-83 Plus"));
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-85"));
            Directory.CreateDirectory(Path.Combine(templatesdir, "TI-86"));

            Directory.CreateDirectory(Path.Combine(Path.Combine(templatesdir, "TI-83 Plus"), "App"));
            Directory.CreateDirectory(Path.Combine(Path.Combine(templatesdir, "TI-83 Plus"), "Mirage OS"));
            Directory.CreateDirectory(Path.Combine(Path.Combine(templatesdir, "TI-83 Plus"), "Ion"));
            Directory.CreateDirectory(Path.Combine(Path.Combine(templatesdir, "TI-83 Plus"), "No Stub"));
        }

        private void browseLoc_Click(object sender, EventArgs e)
        {
            DialogResult browseFile = folderBrowserDialog.ShowDialog();
            if (browseFile == DialogResult.OK)
                locTextBox.Text = folderBrowserDialog.SelectedPath;
        }

        string filePath;
        private void okTemplate_Click(object sender, EventArgs e)
        {
            if (nameTextBox.Text != "")
            {
                string extensionName = "0";
                //ArrayList fileList = new ArrayList();
                //ArrayList imageList = new ArrayList();
                //ArrayList includeList = new ArrayList();
                filePath = locTextBox.Text;
                extenstions = fileTypesBox.Text;
                //string temp = filePath.Substring(0, filePath.LastIndexOf('\\'));
                if (!Directory.Exists(filePath) && !projFromDirBox.Checked)
                    Directory.CreateDirectory(filePath);
                //if (temp.Substring(temp.LastIndexOf('\\') + 1, temp.Length - temp.LastIndexOf('\\') - 1) == nameTextBox.Text || projFromDirBox.Checked)
                //    filePath = filePath.Substring(0, filePath.LastIndexOf('\\'));
                XmlTextWriter writer = new XmlTextWriter(filePath + "\\" + nameTextBox.Text + ".wcodeproj", Encoding.UTF8)
                                           {Formatting = Formatting.Indented};
                //getFiles(filePath, ref fileList, ref imageList, ref includeList);
                writer.WriteStartDocument();
                writer.WriteStartElement("wcodeproj");
                writer.WriteAttributeString("name", nameTextBox.Text + ".wcodeproj");
                writer.WriteStartElement("filesystem");
                    //writer.WriteStartElement("dir");
                        writer.WriteAttributeString("name", nameTextBox.Text);
                        writer.WriteStartElement("dir");
                            writer.WriteAttributeString("name", "Folderz4L33T\\");
                            //foreach (string image in imageList)
                            //{
                            //    writer.WriteStartElement("file");
                            //    writer.WriteAttributeString("Path", image);
                            //    writer.WriteString(image.Remove(0, image.LastIndexOf('\\') + 1));
                            //    writer.WriteEndElement();
                            //}
                        writer.WriteEndElement();
                        /*writer.WriteStartElement("dir");
                            writer.WriteAttributeString("name", "Includes\\");
                            foreach (string include in includeList)
                            {
                                writer.WriteStartElement("file");
                                writer.WriteAttributeString("Path", include);
                                writer.WriteString(include.Remove(0, include.LastIndexOf('\\') + 1));
                                writer.WriteEndElement();
                            }
                        writer.WriteEndElement();
                        writer.WriteStartElement("dir");
                            writer.WriteAttributeString("name", "Output\\");
                        writer.WriteEndElement();
                        foreach (string file in fileList)
                        {
                            writer.WriteStartElement("file");
                            writer.WriteAttributeString("Path", file);
                            writer.WriteString(file.Remove(0, file.LastIndexOf('\\') + 1));
                            writer.WriteEndElement();
                        }*/
                    //writer.WriteEndElement();
                writer.WriteEndElement();
                writer.WriteStartElement("BuildSeq");
                    writer.WriteStartElement("Debug");
                        writer.WriteStartElement("Assemble");
                        writer.WriteAttributeString("action", "Compile");
                        writer.WriteAttributeString("type", extensionName);
                        writer.WriteString(Path.Combine(filePath, nameTextBox.Text + ".asm"));
                        writer.WriteEndElement();
                    writer.WriteEndElement();
                    writer.WriteStartElement("Release");
                        writer.WriteStartElement("Assemble");
                        writer.WriteAttributeString("action", "Compile");
                        writer.WriteAttributeString("type", extensionName);
                        writer.WriteString(Path.Combine(filePath, nameTextBox.Text + ".asm"));
                        writer.WriteEndElement();
                    writer.WriteEndElement();
                writer.WriteEndElement();
                writer.Flush();
                writer.Close();
                projectFile = Path.Combine(filePath, nameTextBox.Text + ".wcodeproj");
                ProjectService.CreateNewProject(projectFile);
                //project = new NewProject(Path.Combine(filePath, nameTextBox.Text + ".wcodeproj"));
                projectLength = filePath.Length;
                getFiles(filePath);
				string[] includeDirs = (string[])includes.ToArray(typeof(string));
                ProjectService.IncludeDirs = includeDirs;

                ProjectService.DeleteFolder("", "Folderz4L33T\\");
                ProjectService.SaveProject();
            }
            else
            {
                MessageBox.Show("No project name entered!");
                cancelQuit = true;
            }
        }
        bool cancelQuit;

        //private void getFiles(string directory, ref ArrayList files, ref ArrayList sprites, ref ArrayList includes)
        //{
        //    foreach (string dir in Directory.GetDirectories(directory))
        //        getFiles(dir, ref files, ref sprites, ref includes);
        //    foreach (string file in Directory.GetFiles(directory))
        //        if (file.EndsWith(".inc"))
        //            includes.Add(file.Remove(0, filePath.Length));
        //        else if (file.EndsWith(".bmp"))
        //            sprites.Add(file.Remove(0, filePath.Length));
        //        else if (file.EndsWith(".asm") || file.EndsWith("*.z80"))
        //            files.Add(file.Remove(0, filePath.Length));
        //}
//        private ProjectClass project;
        private int projectLength;
        private string extenstions;
		private ArrayList includes = new ArrayList();
        private void getFiles(string directory)
        {
            foreach (string dir in Directory.GetDirectories(directory))
            {
				includes.Add(dir);
                string dirName = dir.Substring(dir.LastIndexOf('\\') + 1) + "\\";
                ProjectService.AddDir(dirName, nameTextBox.Text + "\\" + dir.Remove(0, directory.Length + dirName.Length), false);
                getFiles(dir);
            }
            string[] files = Directory.GetFiles(directory);
            string temp;
            foreach (string file in files)
            {
                if (!extenstions.Contains(Path.GetExtension(file)))
                    continue;
                temp = Path.GetDirectoryName(file).Remove(0, projectLength);
                if (temp.Length != 0 && temp[0] == '\\')
                    temp = temp.Substring(1);
                ProjectService.AddFile(Path.GetFileName(file), Path.Combine(nameTextBox.Text, temp) + "\\", Path.Combine("\\" + temp, Path.GetFileName(file)));
            }
        }

        private void templateForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!cancelQuit) return;
            e.Cancel = true;
            cancelQuit = false;
        }

        private void calcOptions_SelectedIndexChanged(object sender, EventArgs e)
        {
            listView1.Items.Clear();
            getTemplatesInDir();
        }

        private void projFromDirBox_CheckedChanged(object sender, EventArgs e)
        {
            fileTypesBox.Visible = projFromDirBox.Checked;
            fileTypesLabel.Visible = projFromDirBox.Checked;
        }
    }
}
