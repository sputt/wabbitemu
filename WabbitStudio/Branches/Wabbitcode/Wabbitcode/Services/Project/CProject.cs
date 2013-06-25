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

		private bool needsSave = false;
		public bool NeedsSave
		{
			get { return needsSave; }
			set { needsSave = value; }
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

			Settings.Default.includeDir = "";
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
            FileStream stream = null;
            try
            {
                stream = new FileStream(projectFile, FileMode.Open);
                using (XmlTextReader reader = new XmlTextReader(stream) { WhitespaceHandling = WhitespaceHandling.None })
                {
                    stream = null;
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
                }
            }
            finally
            {
                if (stream != null)
                {
                    stream.Dispose();
                }
            }
		}

		#region XML

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
					folder.AddFolder(subFolder);
					RecurseReadFolders(reader, ref subFolder);
				}
				else if (reader.Name == "File")
				{
					ProjectFile file = new ProjectFile(this, reader.GetAttribute("Path"));
					folder.AddFile(file);
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

		#endregion

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


		public ProjectFolder AddFolder(string dirName, ProjectFolder parentFolder)
		{
			ProjectFolder folder = new ProjectFolder(this, dirName);
			parentFolder.AddFolder(folder);
			return folder;
		}

		public ProjectFile AddFile(ProjectFolder parentFolder, string fullPath)
		{
			ProjectFile file = new ProjectFile(this, fullPath);
			parentFolder.AddFile(file);
			return file;
		}

		public bool ContainsFile(string fullPath)
		{
			fileFound = null;
			return RecurseSearchFolders(MainFolder, Path.GetFileName(fullPath));
		}

		private ProjectFile fileFound;
		private bool RecurseSearchFolders(ProjectFolder folder, string file)
		{
			if (fileFound == null)
				fileFound = folder.FindFile(file);
			if (fileFound != null)
				return true;
			bool returnVal = false;
			foreach (ProjectFolder subFolder in folder.Folders)
				returnVal |= RecurseSearchFolders(subFolder, file);
			if (fileFound != null)
				return true;
			return returnVal;
		}

		public ProjectFile FindFile(string fullPath)
		{
			if (fileFound != null && string.Equals(fileFound.FileFullPath, fullPath, StringComparison.OrdinalIgnoreCase))
				return fileFound;
			fileFound = null;
			if (ContainsFile(fullPath))
				return fileFound;
			return null;
		}

		public void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir)
		{
			parentDir.Folders.Remove(dir);
		}

		public void DeleteFile(ProjectFolder parentDir, ProjectFile file)
		{
			file.Remove();
		}

		internal int GetOutputType()
		{
			foreach(IBuildStep step in buildSystem.CurrentConfig.Steps){
				if (step.GetType() == typeof(InternalBuildStep))
				{
					string outputFile = ((InternalBuildStep)step).OutputFile;
					if (Path.GetExtension(outputFile) == ".8xk")
						return 5;
				}
			}
			//otherwise we assume its a program
			return 4;
		}
	}
}
