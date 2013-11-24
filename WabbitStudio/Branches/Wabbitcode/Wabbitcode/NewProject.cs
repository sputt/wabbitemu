using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;
using System;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using System.Xml;

namespace Revsoft.Wabbitcode
{
	public partial class NewProjectDialog
	{
		private bool _cancelQuit;
	    private readonly IDocumentService _documentService;
		private readonly IProjectService _projectService;

		public NewProjectDialog(IDocumentService documentService, IProjectService projectService)
		{
		    _documentService = documentService;
			_projectService = projectService;

			InitializeComponent();
			CreateTemplates();
			if (!Directory.Exists(FileLocations.ProjectsDir))
			{
				Directory.CreateDirectory(FileLocations.ProjectsDir);
			}

			try
			{
				ParseTemplatesFile();
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
				Resources.Resources.GetResource("Templates.xml", Path.Combine(FileLocations.TemplatesDir, "Templates.xml"));
				Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-73"));

				// Resources.GetResource("TemplatesSource.nostub73.asm", Path.Combine(templatesDir, "NoStub.asm"));
				Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-82"));

				// Resources.GetResource("TemplatesSource.nostub82.asm", Path.Combine(templatesDir, "NoStub.asm"));
				Resources.Resources.GetResource("TemplatesSource.crash.asm", Path.Combine(FileLocations.TemplatesDir, "TI-82\\CrASH.asm"));
				Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-83"));
				Resources.Resources.GetResource("TemplatesSource.nostub83.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83\\NoStub.asm"));
				Resources.Resources.GetResource("TemplatesSource.venus.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83\\NoStub.asm"));
				Resources.Resources.GetResource("TemplatesSource.ion83.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83\\NoStub.asm"));
				Resources.Resources.GetResource("TemplatesSource.ashell.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83\\AShell.asm"));
				Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus"));
				Resources.Resources.GetResource("TemplatesSource.nostub83p.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus\\NoStub.asm"));
				Resources.Resources.GetResource("TemplatesSource.ion83p.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus\\Ion.asm"));
				Resources.Resources.GetResource("TemplatesSource.mirage.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus\\Mirage.asm"));
				Resources.Resources.GetResource("TemplatesSource.dcs.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus\\DoorsCS.asm"));
				Resources.Resources.GetResource("TemplatesSource.app.asm", Path.Combine(FileLocations.TemplatesDir, "TI-83 Plus\\App.asm"));
				Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-85"));

				// Resources.GetResource("Templates Source.nostub85.asm", Path.Combine(templatesDir, "NoStub.asm"));
				Directory.CreateDirectory(Path.Combine(FileLocations.TemplatesDir, "TI-86"));

				// Resources.GetResource("Templates Source.nostub86.asm", Path.Combine(templatesDir, "NoStub.asm"));

				// Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "App"));
				// Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "Mirage OS"));
				// Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "Ion"));
				// Directory.CreateDirectory(Path.Combine(Path.Combine(templatesDir, "TI-83 Plus"), "No Stub"));

				Directory.CreateDirectory(FileLocations.IncludesDir);
				Resources.Resources.GetResource("TemplatesIncludes.ti73.inc", Path.Combine(FileLocations.IncludesDir, "ti73.inc"));
				Resources.Resources.GetResource("TemplatesIncludes.ti83plus.inc", Path.Combine(FileLocations.IncludesDir, "ti83plus.inc"));
				Resources.Resources.GetResource("TemplatesIncludes.relocate.inc", Path.Combine(FileLocations.IncludesDir, "relocate.inc"));
				Resources.Resources.GetResource("TemplatesIncludes.app.inc", Path.Combine(FileLocations.IncludesDir, "app.inc"));
				Resources.Resources.GetResource("TemplatesIncludes.z80ext.inc", Path.Combine(FileLocations.IncludesDir, "z80ext.inc"));
				Resources.Resources.GetResource("TemplatesIncludes.var.inc", Path.Combine(FileLocations.IncludesDir, "var.inc"));
				Resources.Resources.GetResource("TemplatesIncludes.ion.inc", Path.Combine(FileLocations.IncludesDir, "ion.inc"));
				Resources.Resources.GetResource("TemplatesIncludes.mirage.inc", Path.Combine(FileLocations.IncludesDir, "mirage.inc"));
				Resources.Resources.GetResource("TemplatesIncludes.dcs.inc", Path.Combine(FileLocations.IncludesDir, "dcs.inc"));
				Resources.Resources.GetResource("TemplatesIncludes.keys82.inc", Path.Combine(FileLocations.IncludesDir, "keys82.inc"));
				Resources.Resources.GetResource("TemplatesIncludes.ti82.h", Path.Combine(FileLocations.IncludesDir, "ti82.h"));
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error creating includes and templates", ex);
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

				_projectService.Project.IncludeDirs.Add(dir);
				string dirName = Path.GetFileName(dir);
				ProjectFolder folderAdded = _projectService.AddFolder(dirName, currentFolder);
				GetFiles(dir, extensions, ref folderAdded);
			}

			string[] files = Directory.GetFiles(directory);
			foreach (string file in files.Where(file => file != null && extensions.Contains(Path.GetExtension(file))))
			{
				_projectService.AddFile(currentFolder, file);
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
			ListBox box = new ListBox
			{
				Name = "templatesBox",
				Dock = DockStyle.Fill
			};
			while (reader.Read())
			{
				if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "model")
				{
					break;
				}
				if (reader.Name == "template" && reader.NodeType == XmlNodeType.Element)
				{
					var templateName = reader.GetAttribute("name");
					var ext = reader.GetAttribute("ext");
					if (!reader.Read())
					{
						throw new InvalidDataException("Invalid XML: Unexpected file end");
					}

					var file = reader.Value;
					box.Items.Add(new ListBoxItem
					{
						Text = templateName,
						File = Path.Combine(
							Path.Combine(FileLocations.TemplatesDir, modelName), file),
						Ext = ext
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
				_cancelQuit = true;
				return;
			}

			var item = (ListBoxItem)listBox.SelectedItem;
			string outputExt = item.Ext;
			if (string.IsNullOrEmpty(projectDir))
			{
				DockingService.ShowError("Project directory cannot be empty. Please specify a path for your project");
				_cancelQuit = true;
				return;
			}

			if (string.IsNullOrEmpty(projectName))
			{
				DockingService.ShowError("Project name cannot be empty. Please enter a name for your project");
				_cancelQuit = true;
				return;
			}

			_cancelQuit = false;
			if (!projFromDirBox.Checked)
			{
				projectDir = Path.Combine(projectDir, projectName);
			}

			string projectFile = Path.Combine(projectDir, projectName) + ".wcodeproj";
			if (!Directory.Exists(projectDir))
			{
				Directory.CreateDirectory(projectDir);
			}

			_projectService.CreateNewProject(projectFile, projectName);

			var folder = _projectService.Project.MainFolder;
			if (projFromDirBox.Checked)
			{
				GetFiles(projectDir, fileTypesBox.Text, ref folder);
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
					{
						if (MessageBox.Show("'" + projectName + ".asm'  already exists. Overwrite?", "Error", MessageBoxButtons.YesNo,
								MessageBoxIcon.None) == DialogResult.Yes)
						{
							File.Copy(item.File, mainFile, true);
						}
					}
				}

				_projectService.AddFile(folder, mainFile);
				_documentService.OpenDocument(mainFile);
			}

			_projectService.Project.IncludeDirs.Add(FileLocations.IncludesDir);
			var debug = _projectService.Project.BuildSystem.BuildConfigs.Single(c => c.Name == "Debug");
			var release = _projectService.Project.BuildSystem.BuildConfigs.Single(c => c.Name == "Release");
			debug.Steps.Add(new InternalBuildStep(0, BuildStepType.All, Path.Combine(projectDir, projectName) + ".asm", Path.Combine(projectDir, projectName) + outputExt));
			release.Steps.Add(new InternalBuildStep(0, BuildStepType.Assemble, Path.Combine(projectDir, projectName) + ".asm", Path.Combine(projectDir, projectName) + outputExt));
			_projectService.SaveProject();
			Close();
		}

		private void ParseTemplatesFile()
		{
			var reader = new XmlTextReader(Path.Combine(FileLocations.TemplatesDir, "Templates.xml"))
			{
				WhitespaceHandling = WhitespaceHandling.None
			};
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
						HandleModelNode(ref reader);
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
			if (!_cancelQuit)
			{
				return;
			}

			e.Cancel = true;
			_cancelQuit = false;
		}
	}
}