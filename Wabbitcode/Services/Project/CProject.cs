using System;
using System.Collections;
using System.IO;
using System.Windows.Forms;
using System.Xml;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Classes;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Project
{
    public class ProjectClass : IProject
    {
		private string projectDirectory;
		public string ProjectDirectory
		{
			get { return projectDirectory; }
		}

		private string projectFile;
		public string ProjectFile
		{
			get { return projectFile; }
		}

		private ProjectFolder mainFolder;
		public ProjectFolder MainFolder
		{
			get { return mainFolder; }
			set { mainFolder = value; }
		}

		private string projectName;
		public string ProjectName
		{
			get { return projectName; }
			set { projectName = value; }
		}

		private BuildSystem buildSystem = new BuildSystem(false);
		public BuildSystem BuildSystem
		{
			get { return buildSystem; }
		}

		List<string> includeDir = new List<string>();
		public List<string> IncludeDir
		{
			get { return includeDir; }
			set { includeDir = value; }
		}

		List<string> projectOutputs = new List<string>();
		public List<string> ProjectOutputs
		{
			get { return projectOutputs; }
		}

		List<string> listOutputs = new List<string>();
		public List<string> ListOutputs
		{
			get { return listOutputs; }
		}

		List<string> labelOutputs = new List<string>();
		public List<string> LabelOutputs
		{
			get { return labelOutputs; }
		}

		public ProjectClass()
		{
			
		}

        public ProjectClass(string projectFile)
        {
			ProjectService.IsInternal = false;
			this.projectFile = projectFile;
			projectDirectory = Path.GetDirectoryName(projectFile);
			//mainFolder = new ProjectFolder();

            Settings.Default.includeDir = "";

            if (Settings.Default.startupProject != projectFile)
                if (MessageBox.Show("Would you like to make this your default project?", "Startup Project",
                                    MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                    Settings.Default.startupProject = projectFile;
        }

		internal void CreateNewProject(string projectFile, string projectName)
		{
			ProjectFolder folder = new ProjectFolder(this, projectName);
			this.mainFolder = folder;
			this.projectName = projectName;
			this.projectFile = projectFile;
			this.projectDirectory = Path.GetDirectoryName(projectFile);
			this.buildSystem = new BuildSystem(true);
		}

		internal void OpenProject(string projectFile)
		{
			FileStream stream = new FileStream(projectFile, FileMode.Open);
			XmlTextReader reader = new XmlTextReader(stream);
			reader.WhitespaceHandling = WhitespaceHandling.None;
			reader.MoveToContent();
			while (!reader.Name.Equals("WabbitcodeProject"))
			{
				if (!reader.MoveToNextElement())
					throw new ArgumentException("Invalid XML Format");
			}

			string formatVersion = reader.GetAttribute("Version");
			DialogResult result = DialogResult.Yes;
			if (formatVersion != ProjectFileVersion)
				result = MessageBox.Show("Project Version is not up to date.\nTry to load anyway?", "Invalid Version", MessageBoxButtons.YesNo);
			if (result != DialogResult.Yes)
				return;
			projectName = reader.GetAttribute("Name");
			reader.MoveToNextElement();
			if (reader.Name != "Folder")
				throw new ArgumentException("Invalid XML Format");
			mainFolder = new ProjectFolder(this, reader.GetAttribute("Name"));
			RecurseReadFolders(reader, ref mainFolder);
			buildSystem.ReadXML(reader);
			reader.Close();
			stream.Close();
		}

		private void RecurseReadFolders(XmlTextReader reader, ref ProjectFolder folder)
		{
			if (reader.IsEmptyElement)
				return;
			while (reader.Read())
			{
				if (reader.NodeType == XmlNodeType.EndElement)
					return;
				if (reader.Name == "Folder")
				{
					ProjectFolder subFolder = new ProjectFolder(this, reader.GetAttribute("Name"));
					folder.Folders.Add(subFolder);
					RecurseReadFolders(reader, ref subFolder);
				}
				else if (reader.Name == "File")
				{
					ProjectFile file = new ProjectFile(this, reader.GetAttribute("Path"));
					folder.Files.Add(file);
				} 
				else
					return;
			}
			
		}

		const string ProjectFileVersion = "1.0";
		internal void BuildXMLFile()
		{
			XmlTextWriter writer = new XmlTextWriter(projectFile, Encoding.Unicode);
			writer.Formatting = Formatting.Indented;
			writer.WriteStartDocument();
			writer.WriteComment("Wabbitcode Config File");
			writer.WriteStartElement("WabbitcodeProject");
			writer.WriteAttributeString("Version", ProjectFileVersion);
			writer.WriteAttributeString("Name", projectName);
			RecurseWriteFolders(writer, mainFolder);
			buildSystem.CreateXML(writer);
			writer.WriteEndElement();
			writer.Flush();
			writer.Close();
		}

		private void RecurseWriteFolders(XmlTextWriter writer, ProjectFolder folder)
		{
			writer.WriteStartElement("Folder");
			writer.WriteAttributeString("Name", folder.Name);
			foreach (ProjectFolder subFolder in folder.Folders)
				RecurseWriteFolders(writer, subFolder);
			foreach (ProjectFile file in folder.Files)
			{
				writer.WriteStartElement("File");
				writer.WriteAttributeString("Path", file.FileRelativePath);
				writer.WriteAttributeString("Foldings", file.FileFoldings);
				writer.WriteEndElement();
			}
			writer.WriteEndElement();
		}

		internal List<ProjectFile> GetProjectFiles()
		{
			List<ProjectFile> files = new List<ProjectFile>();
			RecurseAddFiles(ref files, mainFolder);
			return files;
		}

		private void RecurseAddFiles(ref List<ProjectFile> files, ProjectFolder folder)
		{
			foreach (ProjectFolder subFolder in folder.Folders)
				RecurseAddFiles(ref files, subFolder);
			foreach (ProjectFile file in folder.Files)
				files.Add(file);
		}
	}
}
