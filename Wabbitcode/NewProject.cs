using System;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using Revsoft.Wabbitcode.Classes;
using System.Collections;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode
{
    public partial class NewProjectDialog
    {
        public string projectFile = "";
        public NewProjectDialog()
        {
            InitializeComponent();
            CreateTemplates();
            if (!Directory.Exists(FileLocations.ProjectsDir))
                Directory.CreateDirectory(FileLocations.ProjectsDir);
            GetTemplates();
            calcOptions.SelectedIndex = 0;
            GetTemplatesInDir();
        }

        private void GetTemplates()
        {
            calcOptions.Items.Add("Binary");
            string[] dirs = Directory.GetDirectories(FileLocations.TemplatesDir);
            foreach (string dir in dirs)
                calcOptions.Items.Add(Path.GetFileName(dir));
        }

        private void GetTemplatesInDir()
        {
            if (calcOptions.SelectedIndex == 0)
                return;
            string[] templates = Directory.GetFiles(Path.Combine(FileLocations.TemplatesDir, calcOptions.Items[calcOptions.SelectedIndex].ToString()));
            foreach (string template in templates)
                listView1.Items.Add(Path.GetFileNameWithoutExtension(template));
        }

        private void CreateTemplates()
        {
#if !DEBUG
            try
            {
#endif
                //Uncomment these as they get added in
                Directory.CreateDirectory(FileLocations.TemplatesDir);
                Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-73"));
                //Resources.GetResource("TemplatesSource.nostub73.asm", Path.Combine(templatesDir, "NoStub.asm"));
                Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-82"));
                //Resources.GetResource("TemplatesSource.nostub82.asm", Path.Combine(templatesDir, "NoStub.asm"));
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
                //Resources.GetResource("Templates Source.nostub85.asm", Path.Combine(templatesDir, "NoStub.asm"));
                Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-86"));
                //Resources.GetResource("Templates Source.nostub86.asm", Path.Combine(templatesDir, "NoStub.asm"));

                //Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "App"));
                //Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "Mirage OS"));
                //Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "Ion"));
                //Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "No Stub"));

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
#if !DEBUG
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error creating includes and templates" + ex.ToString());
            }
#endif
        }

        private void browseLoc_Click(object sender, EventArgs e)
        {
            folderBrowserDialog.SelectedPath = FileLocations.ProjectsDir;
            DialogResult browseFile = folderBrowserDialog.ShowDialog();
            if (browseFile == DialogResult.OK)
                locTextBox.Text = folderBrowserDialog.SelectedPath;
        }

        string filePath;
        private void okTemplate_Click(object sender, EventArgs e)
        {
			string projectName = nameTextBox.Text;
            if (projectName != "")
            {
                string extensionName = "8xk";
                if (projFromDirBox.Checked)
                    filePath = locTextBox.Text;
                else
                    filePath = Path.Combine(locTextBox.Text, projectName);
                extensions = fileTypesBox.Text;
                if (!Directory.Exists(filePath))
                    if (!projFromDirBox.Checked)
                        Directory.CreateDirectory(filePath);
                    else
                    {
                        DockingService.ShowError("Directory does not exist");
                        return;
                    }
                projectFile = Path.Combine(filePath, projectName + ".wcodeproj");
				ProjectService.CreateNewProject(projectFile, projectName);
				ProjectFolder currentFolder = ProjectService.MainFolder;
                string fileDestination = Path.Combine(filePath, projectName + ".asm");
                if (!projFromDirBox.Checked)
                {
                    string fileToCopy = Path.Combine(Path.Combine(FileLocations.TemplatesDir, calcOptions.SelectedItem.ToString()), listView1.SelectedItems[0].Text + ".asm");
                    if (File.Exists(fileDestination))
                        if (MessageBox.Show("File already exists, overwrite?", "Overwrite", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation) == DialogResult.Yes)
                            File.Copy(fileToCopy, fileDestination, true);
                        else
                            return;
                    else
                        File.Copy(fileToCopy, fileDestination, true);
                    ProjectFile file = new ProjectFile(ProjectService.Project, fileDestination);
                    currentFolder.AddFile(file);
                }
                else
                {
#if !DEBUG
                    try
                    {
#endif
                        GetFiles(filePath, ref currentFolder);
#if !DEBUG
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Problem getting old files: " + ex.ToString());
                    }
#endif
                }
                IBuildStep stepToAdd = new InternalBuildStep(0, StepType.Assemble, fileDestination, Path.ChangeExtension(fileDestination, extensionName));
                //debug
                ProjectService.BuildConfigs[0].Steps.Add(stepToAdd);
                //release
                ProjectService.BuildConfigs[1].Steps.Add(stepToAdd);
                ProjectService.IncludeDirs.Add(FileLocations.IncludesDir);
                ProjectService.SaveProject();
            }
            else
            {
                MessageBox.Show("No project name entered!");
                cancelQuit = true;
            }
        }
        bool cancelQuit;

        private string extensions;
        private void GetFiles(string directory, ref ProjectFolder currentFolder)
        {
            foreach (string dir in Directory.GetDirectories(directory))
            {
                if ((new DirectoryInfo(dir).Attributes & FileAttributes.Hidden) == FileAttributes.Hidden)
                    continue;
				ProjectService.IncludeDirs.Add(dir);
				string dirName = Path.GetFileName(dir);
				ProjectFolder folderAdded = ProjectService.AddFolder(dirName, currentFolder);
                GetFiles(dir, ref folderAdded);
            }
            string[] files = Directory.GetFiles(directory);
            foreach (string file in files)
            {
                if (!extensions.Contains(Path.GetExtension(file)))
                    continue;
				ProjectService.AddFile(currentFolder, file);
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
            if (calcOptions.SelectedIndex == 0)
                listView1.Items.Add("Binary");
            else
                GetTemplatesInDir();
        }

        private void projFromDirBox_CheckedChanged(object sender, EventArgs e)
        {
            fileTypesBox.Visible = projFromDirBox.Checked;
            fileTypesLabel.Visible = projFromDirBox.Checked;
        }
    }
}
