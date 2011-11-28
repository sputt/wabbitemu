using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Interface.Services;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project.Interface;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class Project : IProject
	{
		/// <summary>
		/// Root directory that the project resides in.
		/// </summary>
		public string ProjectDirectory { get; private set; }

		/// <summary>
		/// Location of the actual XML file of the project. 
		/// Normally located in $(ProjectDirectory)\$(ProjectName).wcodeproj
		/// </summary>
		public string ProjectFile { get; private set; }

		/// <summary>
		/// Root folder of the virtual folder Wabbitcode uses to keep track of files
		/// </summary>
		public IProjectFolder MainFolder { get; set; }

		/// <summary>
		/// Name of the project
		/// </summary>
		public string ProjectName { get; set; }

		/// <summary>
		/// Whether we've made changes to the project file that need to be saved
		/// </summary>
		public bool NeedsSave { get; set; }

		/// <summary>
		/// Class for building the project. Contains the build configs and each build step is contained within them
		/// </summary>
		public IBuildSystem BuildSystem { get; private set; }

		public IParserService ParserService { get; private set; }

		public IAssemblerService AssemblerService { get; private set; }

		/// <summary>
		/// List of all the paths to include when building so files are properly found
		/// </summary>
		public IList<string> IncludeDirs { get; set; }

		/// <summary>
		/// List of all the exe files the project outputs
		/// </summary>
		public IList<string> ProjectOutputs { get; private set; }

		/// <summary>
		/// List of all the listing files the project outputs
		/// </summary>
		public IList<string> ListOutputs { get; private set; }

		/// <summary>
		/// List of all the label files the project outputs
		/// </summary>
		public IList<string> LabelOutputs { get; private set; }

		Project(IAssemblerService assemblerService, IParserService parserService) 
		{
			BuildSystem = new BuildSystem(this, false);
			this.IncludeDirs = new List<string>();

			ProjectOutputs = new List<string>();
			LabelOutputs = new List<string>();
			ListOutputs = new List<string>();

			AssemblerService = assemblerService;
			ParserService = parserService;
		}

		public Project()
		{
			ParserService = new ParserService();
		}

		public Project(string projectFile, string projectName)
		{
			IProjectFolder folder = new ProjectFolder(this, projectName);
			MainFolder = folder;
			ProjectName = projectName;
			ProjectFile = projectFile;
			ProjectDirectory = Path.GetDirectoryName(projectFile);
			BuildSystem = new BuildSystem(this, true);
			ParserService = new ParserService();
		}

		public Project(Stream stream, string projectFile)
		{
			using (var reader = new XmlTextReader(stream))
			{
				stream = null;
				reader.WhitespaceHandling = WhitespaceHandling.None;
				reader.MoveToContent();
				while (!reader.Name.Equals("WabbitcodeProject"))
				{
                    if (!reader.MoveToNextElement())
                    {
                        throw new InvalidDataException("Invalid XML Format");
                    }
				}

				string formatVersion = reader.GetAttribute("Version");
                if (formatVersion != ProjectFileVersion)
                {
                    throw new Exception("Invalid Version");
                }
				ProjectFile = projectFile;
				ProjectDirectory = reader.GetAttribute("Directory");
                if (ProjectDirectory == null)
                {
                    ProjectDirectory = Path.GetDirectoryName(projectFile);
                }
				ProjectName = reader.GetAttribute("Name");
				reader.MoveToNextElement();
                if (reader.Name != "Folder")
                {
                    throw new ArgumentException("Invalid XML Format");
                }
				IProjectFolder mainFolder = new ProjectFolder(this, reader.GetAttribute("Name"));
				RecurseReadFolders(reader, ref mainFolder);
				MainFolder = mainFolder;
				BuildSystem = new BuildSystem(this, false);
				IncludeDirs = new List<string>();
				BuildSystem.ReadXML(reader);
			}
			ParserService = new ParserService();
		}

		#region XML
		private void RecurseReadFolders(XmlTextReader reader, ref IProjectFolder folder)
		{
			if (reader.IsEmptyElement)
				return;
			while (reader.Read())
			{
				if (reader.NodeType == XmlNodeType.EndElement)
					return;
				if (reader.Name == "Folder")
				{
					IProjectFolder subFolder = new ProjectFolder(this, reader.GetAttribute("Name"));
					folder.AddFolder(subFolder);
					RecurseReadFolders(reader, ref subFolder);
				}
				else if (reader.Name == "File")
				{
					IProjectFile file = new ProjectFile(this, reader.GetAttribute("Path"));
					folder.AddFile(file);
				}
				else
				{
					return;
				}
			}
			
		}

		const string ProjectFileVersion = "1.0";
		public void SaveProjectFile(Stream stream)
		{
			using (XmlTextWriter writer = new XmlTextWriter(stream, Encoding.Unicode))
			{
				writer.Formatting = Formatting.Indented;
				writer.WriteStartDocument();
				writer.WriteComment("Wabbitcode Config File");
				writer.WriteStartElement("WabbitcodeProject");
				writer.WriteAttributeString("Version", ProjectFileVersion);
				writer.WriteAttributeString("Directory", ProjectDirectory);
				writer.WriteAttributeString("Name", ProjectName);
				RecurseWriteFolders(writer, MainFolder);
				BuildSystem.CreateXML(writer);
				writer.WriteEndElement();
				writer.Flush();
			}
		}

		private void RecurseWriteFolders(XmlTextWriter writer, IProjectFolder folder)
		{
			writer.WriteStartElement("Folder");
			writer.WriteAttributeString("Name", folder.Name);
			foreach (IProjectFolder subFolder in folder.Folders)
				RecurseWriteFolders(writer, subFolder);
			foreach (IProjectFile file in folder.Files)
			{
				writer.WriteStartElement("File");
				writer.WriteAttributeString("Path", file.FileRelativePath);
				writer.WriteAttributeString("Foldings", file.FileFoldings);
				writer.WriteEndElement();
			}
			writer.WriteEndElement();
		}
		#endregion
		
		public IList<IProjectFile> GetProjectFiles()
		{
			IList<IProjectFile> files = new List<IProjectFile>();
			RecurseAddFiles(ref files, MainFolder);
			return files;
		}

		private void RecurseAddFiles(ref IList<IProjectFile> files, IProjectFolder folder)
		{
			foreach (IProjectFolder subFolder in folder.Folders)
				RecurseAddFiles(ref files, subFolder);
			foreach (IProjectFile file in folder.Files)
				files.Add(file);
		}


		#region Virtual File Management
		public IProjectFolder AddFolder(string dirName, IProjectFolder parentFolder)
		{
			IProjectFolder folder = new ProjectFolder(this, dirName);
			parentFolder.AddFolder(folder);
			return folder;
		}

		public IProjectFile AddFile(IProjectFolder parentFolder, string fullPath)
		{
			IProjectFile file = new ProjectFile(this, fullPath);
			parentFolder.AddFile(file);
			return file;
		}

		public void DeleteFolder(IProjectFolder parentDir, IProjectFolder dir)
		{
			parentDir.Folders.Remove(dir);
		}

		public void DeleteFile(string fullPath)
		{
			IProjectFile file = FindFile(fullPath);
			DeleteFile(file);
			NeedsSave = true;
		}

		public void DeleteFile(IProjectFile file)
		{
			file.Remove();
		}

		public bool ContainsFile(string fullPath)
		{
			fileFound = null;
			return RecurseSearchFolders(MainFolder, Path.GetFileName(fullPath));
		}

		public IProjectFile FindFile(string fullPath)
		{
			if (fileFound != null && fileFound.FileFullPath == fullPath)
				return fileFound;
			fileFound = null;
			if (ContainsFile(fullPath))
				return fileFound;
			return null;
		}

		private IProjectFile fileFound;
		private bool RecurseSearchFolders(IProjectFolder folder, string file)
		{
			if (fileFound == null)
				fileFound = folder.FindFile(file);
			if (fileFound != null)
				return true;
			bool returnVal = false;
			foreach (IProjectFolder subFolder in folder.Folders)
				returnVal |= RecurseSearchFolders(subFolder, file);
			if (fileFound != null)
				return true;
			return returnVal;
		}

		#endregion

		public class AssemblerOutputType
		{
			readonly string extension;
			public AssemblerOutputType(string ext)
			{
				extension = ext;
			}

			public override bool Equals(object obj)
			{
				AssemblerOutputType output = obj as AssemblerOutputType;
				if (output == null)
				{
					return base.Equals(obj);
				}
				return extension.Equals(output.extension);
			}

			public override int GetHashCode()
			{
				return extension.GetHashCode();
			}

			public override string ToString()
			{
				return extension;
			}
		}

		public AssemblerOutputType GetOutputType()
		{
			foreach (IBuildStep step in BuildSystem.CurrentConfig.Steps)
			{
				if (step is IInternalBuildStep)
				{
					IInternalBuildStep iStep = (IInternalBuildStep)step;
					string outputFile = iStep.OutputFile;
					string ext = Path.GetExtension(outputFile);
					return new AssemblerOutputType(ext);
				}
			}
			//otherwise we assume its a bin
			return new AssemblerOutputType(".bin");
		}
	}
}
