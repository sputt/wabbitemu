namespace Revsoft.Wabbitcode
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.IO;
    using System.Text;
    using System.Windows.Forms;
    using System.Xml;

    using Revsoft.Wabbitcode.Classes;
    using Revsoft.Wabbitcode.Services;
    using Revsoft.Wabbitcode.Services.Project;

    public partial class NewProjectDialog
    {
        public string projectFile = "";

        private bool cancelQuit;

        public NewProjectDialog()
        {
            InitializeComponent();
            this.CreateTemplates();
            if (!Directory.Exists(FileLocations.ProjectsDir))
            {
                Directory.CreateDirectory(FileLocations.ProjectsDir);
            }

            try
            {
                this.ParseTemplatesFile();
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error loading templates file", ex);
            }

            locTextBox.Text = FileLocations.ProjectsDir;
        }

        private void browseLoc_Click(object sender, EventArgs e)
        {
            folderBrowserDialog.SelectedPath = FileLocations.ProjectsDir;
            DialogResult browseFile = folderBrowserDialog.ShowDialog();
            if (browseFile == DialogResult.OK)
            {
                locTextBox.Text = folderBrowserDialog.SelectedPath;
            }
        }

        private void CreateTemplates()
        {
            try
            {
                // Uncomment these as they get added in
                Directory.CreateDirectory(FileLocations.TemplatesDir);
                Resources.GetResource("Templates.xml", Path.Combine(FileLocations.TemplatesDir, "Templates.xml"));
                Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-73"));

                // Resources.GetResource("TemplatesSource.nostub73.asm", Path.Combine(templatesDir, "NoStub.asm"));
                Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-82"));

                // Resources.GetResource("TemplatesSource.nostub82.asm", Path.Combine(templatesDir, "NoStub.asm"));
                Resources.GetResource("TemplatesSource.crash.asm", Path.Combine(FileLocations.TemplatesDir, "TI-82\\CrASH.asm"));
                Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-83"));
                Resources.GetResource("TemplatesSource.nostub83.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83\\NoStub.asm"));
                Resources.GetResource("TemplatesSource.venus.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83\\NoStub.asm"));
                Resources.GetResource("TemplatesSource.ion83.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83\\NoStub.asm"));
                Resources.GetResource("TemplatesSource.ashell.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83\\AShell.asm"));
                Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus"));
                Resources.GetResource("TemplatesSource.nostub83p.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus\\NoStub.asm"));
                Resources.GetResource("TemplatesSource.ion83p.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus\\Ion.asm"));
                Resources.GetResource("TemplatesSource.mirage.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus\\Mirage.asm"));
                Resources.GetResource("TemplatesSource.dcs.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus\\DoorsCS.asm"));
                Resources.GetResource("TemplatesSource.app.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus\\App.asm"));
                Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-85"));

                // Resources.GetResource("Templates Source.nostub85.asm", Path.Combine(templatesDir, "NoStub.asm"));
                Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-86"));

                // Resources.GetResource("Templates Source.nostub86.asm", Path.Combine(templatesDir, "NoStub.asm"));

                // Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "App"));
                // Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "Mirage OS"));
                // Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "Ion"));
                // Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "No Stub"));

                Directory.CreateDirectory(FileLocations.IncludesDir);
                Resources.GetResource("TemplatesIncludes.ti73.inc", Path.Combine(FileLocations.IncludesDir, "ti73.inc"));
                Resources.GetResource("TemplatesIncludes.ti83plus.inc", Path.Combine(FileLocations.IncludesDir, "ti83plus.inc"));
                Resources.GetResource("TemplatesIncludes.relocate.inc", Path.Combine(FileLocations.IncludesDir, "relocate.inc"));
                Resources.GetResource("TemplatesIncludes.app.inc", Path.Combine(FileLocations.IncludesDir, "app.inc"));
                Resources.GetResource("TemplatesIncludes.z80ext.inc", Path.Combine(FileLocations.IncludesDir, "z80ext.inc"));
                Resources.GetResource("TemplatesIncludes.var.inc", Path.Combine(FileLocations.IncludesDir, "var.inc"));
                Resources.GetResource("TemplatesIncludes.ion.inc", Path.Combine(FileLocations.IncludesDir, "ion.inc"));
                Resources.GetResource("TemplatesIncludes.mirage.inc", Path.Combine(FileLocations.IncludesDir, "mirage.inc"));
                Resources.GetResource("TemplatesIncludes.dcs.inc", Path.Combine(FileLocations.IncludesDir, "dcs.inc"));
                Resources.GetResource("TemplatesIncludes.keys82.inc", Path.Combine(FileLocations.IncludesDir, "keys82.inc"));
                Resources.GetResource("TemplatesIncludes.ti82.h", Path.Combine(FileLocations.IncludesDir, "ti82.h"));
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error creating includes and templates" + ex.ToString());
            }
        }

        private void GetFiles(string directory, string extensions, ref ProjectFolder currentFolder)
        {
            foreach (string dir in Directory.GetDirectories(directory))
            {
                if ((new DirectoryInfo(dir).Attributes & FileAttributes.Hidden) == FileAttributes.Hidden)
                {
                    continue;
                }

                ProjectService.IncludeDirs.Add(dir);
                string dirName = Path.GetFileName(dir);
                ProjectFolder folderAdded = ProjectService.AddFolder(dirName, currentFolder);
                this.GetFiles(dir, extensions, ref folderAdded);
            }

            string[] files = Directory.GetFiles(directory);
            foreach (string file in files)
            {
                if (!extensions.Contains(Path.GetExtension(file)))
                {
                    continue;
                }

                ProjectService.AddFile(currentFolder, file);
            }
        }

        private void HandleModelNode(ref XmlTextReader reader)
        {
            var modelName = reader.GetAttribute("name");
            if (modelName == null)
            {
                throw new InvalidDataException("Invalid XML Format: no model name specified");
            }

            TabPage page = new TabPage(modelName);
            tabControl.TabPages.Add(page);
            ListBox box = new ListBox()
            {
                Name = "templatesBox", Dock = DockStyle.Fill
            };
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "model")
                {
                    break;
                }
                else if (reader.Name == "template" && reader.NodeType == XmlNodeType.Element)
                {
                    var templateName = reader.GetAttribute("name");
                    var ext = reader.GetAttribute("ext");
                    if (!reader.Read())
                    {
                        throw new InvalidDataException("Invalid XML: Unexpected file end");
                    }

                    var file = reader.Value;
                    box.Items.Add(new ListBoxItem()
                    {
                        Text = templateName, File = Path.Combine(
                                                        Path.Combine(FileLocations.TemplatesDir, modelName), file), Ext = ext
                    });
                }
            }

            page.Controls.Add(box);
        }

        private void okTemplate_Click(object sender, EventArgs e)
        {
            string projectDir = locTextBox.Text.Trim();
            string projectName = nameTextBox.Text.Trim();
            var listBox = (ListBox)tabControl.SelectedTab.Controls["templatesBox"];
            if (listBox.SelectedItem == null)
            {
                DockingService.ShowError("You must select an output type for this project");
                this.cancelQuit = true;
                return;
            }

            var item = (ListBoxItem)listBox.SelectedItem;
            string outputExt = item.Ext;
            if (string.IsNullOrEmpty(projectDir))
            {
                DockingService.ShowError("Project directory cannot be empty. Please specify a path for your project");
                this.cancelQuit = true;
                return;
            }

            if (string.IsNullOrEmpty(projectName))
            {
                DockingService.ShowError("Project name cannot be empty. Please enter a name for your project");
                this.cancelQuit = true;
                return;
            }

            this.cancelQuit = false;
            if (!projFromDirBox.Checked)
            {
                projectDir = Path.Combine(projectDir, projectName);
            }

            string projectFile = Path.Combine(projectDir, projectName) + ".wcodeproj";
            if (!Directory.Exists(projectDir))
            {
                Directory.CreateDirectory(projectDir);
            }

            ProjectService.CreateNewProject(projectFile, projectName);
            var folder = ProjectService.MainFolder;
            if (projFromDirBox.Checked)
            {
                this.GetFiles(projectDir, fileTypesBox.Text, ref folder);
            }
            else
            {
                string mainFile = Path.Combine(projectDir, projectName + ".asm");
                try
                {
                    File.Copy(item.File, mainFile);
                }
                catch (IOException ex)
                {
                    if (ex.Message.Contains("exists"))
                        if (MessageBox.Show("'" + projectName + ".asm'  already exists. Overwrite?", "Error", MessageBoxButtons.YesNo, MessageBoxIcon.None) == DialogResult.Yes)
                        {

                        }
                    {
                    }

                    {
                    }

                    {
                    }

                    {
                        File.Copy(item.File, mainFile, true);
                    }
                }

                ProjectService.AddFile(folder, mainFile);
                DocumentService.OpenDocument(mainFile);
            }

            ProjectService.IncludeDirs.Add(FileLocations.IncludesDir);
            var debug = ProjectService.BuildConfigs[0];
            var release = ProjectService.BuildConfigs[1];
            debug.Steps.Add(new InternalBuildStep(0, BuildStepType.All, Path.Combine(projectDir, projectName) + ".asm", Path.Combine(projectDir, projectName) + outputExt));
            release.Steps.Add(new InternalBuildStep(0, BuildStepType.Assemble, Path.Combine(projectDir, projectName) + ".asm", Path.Combine(projectDir, projectName) + outputExt));
            ProjectService.SaveProject();
            this.Close();
        }

        private void ParseTemplatesFile()
        {
            var reader = new XmlTextReader(Path.Combine(FileLocations.TemplatesDir, "Templates.xml"));
            reader.WhitespaceHandling = WhitespaceHandling.None;
            reader.MoveToContent();
            if (reader.Name != "templates")
            {
                throw new InvalidDataException("Invalid XML Format: unable to find head node");
            }

            reader.MoveToNextElement();
            while (reader.MoveToNextElement())
            {
                switch (reader.Name)
                {
                case "model":
                    this.HandleModelNode(ref reader);
                    break;
                }
            }
        }

        private void projFromDirBox_CheckedChanged(object sender, EventArgs e)
        {
            fileTypesBox.Visible = projFromDirBox.Checked;
            fileTypesLabel.Visible = projFromDirBox.Checked;
        }

        private void templateForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!this.cancelQuit)
            {
                return;
            }

            e.Cancel = true;
            this.cancelQuit = false;
        }
    }
}