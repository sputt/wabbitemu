using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Xml;
using Revsoft.Wabbitcode.Interface.Services;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode.Dialogs
{
	/// <summary>
	/// Interaction logic for NewProjectDialog.xaml
	/// </summary>
	public partial class NewProjectDialog : Window
	{
		IPathsService pathsService;
		IProjectService projectService;

		public NewProjectDialog()
		{
			pathsService = ServiceFactory.Instance.GetServiceInstance<PathsService>();
			projectService = ServiceFactory.Instance.GetServiceInstance<ProjectService>();
			InitializeComponent();
			ParseTemplatesFile();
			projectDirBox.Text = pathsService.ProjectDirectory;
		}

		void ParseTemplatesFile()
		{
			var reader = new XmlTextReader(pathsService.TemplatesConfig);
			reader.WhitespaceHandling = WhitespaceHandling.None;
			reader.MoveToContent();
			if (reader.Name != "templates")
				throw new InvalidDataException("Invalid XML Format: unable to find head node");
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

		private void HandleModelNode(ref XmlTextReader reader)
		{
			var modelName = reader.GetAttribute("name");
			if (modelName == null)
				throw new InvalidDataException("Invalid XML Format: no model name specified");
			var tabItem = new TabItem()
			{
				Header = modelName,
			};
			var content = new ListView()
			{
				
			};
			while (reader.Read())
			{
				if (reader.NodeType == XmlNodeType.EndElement && reader.Name == "model")
					break;
				else if (reader.Name == "template")
				{
					var templateName = reader.GetAttribute("name");
					var file = reader.Value;
					content.Items.Add(new ListViewItem() { Content = templateName, Tag = file });
				}
			}
			tabItem.Content = content;
			tabControl.Items.Add(tabItem);
		}

		private void cancelButton_Click(object sender, RoutedEventArgs e)
		{
			this.Close();
		}

		private void okButton_Click(object sender, RoutedEventArgs e)
		{
			string projectDir = projectDirBox.Text.Trim().CleanPath();
			string projectName = projectNameBox.Text.Trim().CleanFilename();
			if (string.IsNullOrEmpty(projectDir))
			{
				DockingService.ShowError("Project directory cannot be empty. Please specify a path for your project");
				return;
			}
			if (string.IsNullOrEmpty(projectName))
			{
				DockingService.ShowError("Project name cannot be empty. Please enter a name for your project");
				return;
			}
			string projectFile = projFromExistingCodeBox.IsChecked == true ? projectDir : System.IO.Path.Combine(projectDir, projectName);
			if (!Directory.Exists(projectDir))
				Directory.CreateDirectory(projectDir);

			var project = new Project(projectFile, projectName);
			projectService.OpenProject(projectFile);
			
			this.Close();
		}

		private void browseButton_Click(object sender, RoutedEventArgs e)
		{
			var folder = new System.Windows.Forms.FolderBrowserDialog()
			{
				SelectedPath = projectDirBox.Text,
				Description = "Select a project Folder",
			};
			if (folder.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				projectDirBox.Text = folder.SelectedPath;
			}
		}
	}
}
