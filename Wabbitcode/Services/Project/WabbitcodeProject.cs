using System.Linq;
using Revsoft.Wabbitcode.Extensions;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using Revsoft.Wabbitcode.Services.Interface;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class WabbitcodeProject : IProject
	{
		private const string ProjectFileVersion = "1.0";

		private BuildSystem _buildSystem;
		private ProjectFile _fileFound;
		private readonly List<string> _includeDirs = new List<string>();
		private ProjectFolder _mainFolder;
		private readonly IAssemblerService _assemblerService;

		public WabbitcodeProject(IAssemblerService assemblerService)
		{
			_assemblerService = assemblerService;
			_buildSystem = new BuildSystem(_assemblerService, this);
			ProjectWatcher = new FileSystemWatcher();
			if (!string.IsNullOrEmpty(ProjectDirectory))
			{
				ProjectWatcher.Path = ProjectDirectory;
			}
		}

		public WabbitcodeProject(string projectFile, IAssemblerService assemblerService)
			: this(assemblerService)
		{
			ProjectFile = projectFile;
			ProjectDirectory = Path.GetDirectoryName(projectFile);
		}

		#region Public Members

		public bool IsInternal { get; set; }

		public FileSystemWatcher ProjectWatcher { get; set; }

		public IBuildSystem BuildSystem
		{
			get
			{
				return _buildSystem;
			}
		}

		public IList<string> IncludeDirs
		{
			get
			{
				return _includeDirs;
			}
		}

		public bool NeedsSave { get; private set; }

		public string ProjectDirectory { get; private set; }

		public string ProjectFile { get; private set; }

		public string ProjectName { get; set; }

		public ProjectFolder MainFolder
		{
			get
			{
				return _mainFolder;
			}
		}

		#endregion

		public ProjectFile AddFile(ProjectFolder parentFolder, string fullPath)
		{
			ProjectFile file = new ProjectFile(this, fullPath);
			parentFolder.AddFile(file);
			NeedsSave = true;
			return file;
		}

		public ProjectFolder AddFolder(string dirName, ProjectFolder parentFolder)
		{
			ProjectFolder folder = new ProjectFolder(this, dirName);
			parentFolder.AddFolder(folder);
			return folder;
		}

		public bool ContainsFile(string fullPath)
		{
			_fileFound = null;
			return RecurseSearchFolders(MainFolder, Path.GetFileName(fullPath));
		}

		public void DeleteFile(ProjectFolder parentDir, ProjectFile file)
		{
			file.Remove();
			NeedsSave = true;
		}

		public void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir)
		{
			parentDir.Folders.Remove(dir);
			NeedsSave = true;
		}

		public ProjectFile FindFile(string fullPath)
		{
			if (_fileFound != null && string.Equals(_fileFound.FileFullPath, fullPath, StringComparison.OrdinalIgnoreCase))
			{
				return _fileFound;
			}
			_fileFound = null;
			return ContainsFile(fullPath) ? _fileFound : null;
		}

		internal void BuildXMLFile()
		{
			XmlTextWriter writer = new XmlTextWriter(ProjectFile, Encoding.Unicode)
			{
				Formatting = Formatting.Indented
			};
			writer.WriteStartDocument();
			writer.WriteComment("Wabbitcode Config File");
			writer.WriteStartElement("WabbitcodeProject");
			writer.WriteAttributeString("Version", ProjectFileVersion);
			writer.WriteAttributeString("Name", ProjectName);
			RecurseWriteFolders(writer, _mainFolder);
			_buildSystem.WriteXML(writer);
			writer.WriteEndElement();
			writer.Flush();
			writer.Close();
		}

		internal void CreateNewProject(string projectFile, string projectName)
		{
			ProjectFolder folder = new ProjectFolder(this, projectName);
			_mainFolder = folder;
			ProjectName = projectName;
			ProjectFile = projectFile;
			ProjectDirectory = Path.GetDirectoryName(projectFile);

			_buildSystem = new BuildSystem(_assemblerService, this);
			_buildSystem.CreateDefaultConfigs();
		}

		public IEnumerable<ProjectFile> GetProjectFiles()
		{
			List<ProjectFile> files = new List<ProjectFile>();
			RecurseAddFiles(ref files, _mainFolder);
			return files;
		}

		public void SaveProject()
		{
			BuildXMLFile();
			NeedsSave = false;
		}

		internal void OpenProject(string projectFile)
		{
			FileStream stream = null;
			try
			{
				stream = new FileStream(projectFile, FileMode.Open);
				using (XmlTextReader reader = new XmlTextReader(stream)
				{
					WhitespaceHandling = WhitespaceHandling.None
				})
				{
					stream = null;
					reader.MoveToContent();
					while (!reader.Name.Equals("WabbitcodeProject"))
					{
						if (!reader.MoveToNextElement())
						{
							throw new ArgumentException("Invalid XML Format");
						}
					}

					string formatVersion = reader.GetAttribute("Version");
					DialogResult result = DialogResult.Yes;
					if (formatVersion != ProjectFileVersion)
					{
						result = MessageBox.Show("Project Version is not up to date.\nTry to load anyway?", "Invalid Version", MessageBoxButtons.YesNo);
					}
					if (result != DialogResult.Yes)
					{
						return;
					}
					ProjectName = reader.GetAttribute("Name");
					reader.MoveToNextElement();
					if (reader.Name != "Folder")
					{
						throw new ArgumentException("Invalid XML Format");
					}
					_mainFolder = new ProjectFolder(this, reader.GetAttribute("Name"));
					RecurseReadFolders(reader, ref _mainFolder);
					_buildSystem.ReadXML(reader);
				}
			}
			finally
			{
				if (stream != null)
				{
					stream.Dispose();
				}
			}
			ProjectWatcher.Path = ProjectDirectory;
		}

		private void RecurseAddFiles(ref List<ProjectFile> files, ProjectFolder folder)
		{
			foreach (ProjectFolder subFolder in folder.Folders)
			{
				RecurseAddFiles(ref files, subFolder);
			}
			files.AddRange(folder.Files);
		}

		private void RecurseReadFolders(XmlTextReader reader, ref ProjectFolder folder)
		{
			if (reader.IsEmptyElement)
			{
				return;
			}
			while (reader.Read())
			{
				if (reader.NodeType == XmlNodeType.EndElement)
				{
					return;
				}
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
				{
					return;
				}
			}
		}

		private bool RecurseSearchFolders(ProjectFolder folder, string file)
		{
			if (_fileFound == null)
			{
				_fileFound = folder.FindFile(file);
			}
			if (_fileFound != null)
			{
				return true;
			}
			bool returnVal = folder.Folders.Aggregate(false, (current, subFolder) => current | RecurseSearchFolders(subFolder, file));
			if (_fileFound != null)
			{
				return true;
			}
			return returnVal;
		}

		private void RecurseWriteFolders(XmlTextWriter writer, ProjectFolder folder)
		{
			writer.WriteStartElement("Folder");
			writer.WriteAttributeString("Name", folder.Name);
			foreach (ProjectFolder subFolder in folder.Folders)
			{
				RecurseWriteFolders(writer, subFolder);
			}
			foreach (ProjectFile file in folder.Files)
			{
				writer.WriteStartElement("File");
				writer.WriteAttributeString("Path", file.FileRelativePath);
				writer.WriteAttributeString("Foldings", file.FileFoldings);
				writer.WriteEndElement();
			}

			writer.WriteEndElement();
		}

		public void InitWatcher(FileSystemEventHandler changedHandler, RenamedEventHandler renamedHandler)
		{
			ProjectWatcher.Changed += changedHandler;
			ProjectWatcher.Renamed += renamedHandler;
			if (!IsInternal)
			{
				ProjectWatcher.EnableRaisingEvents = true;
				ProjectWatcher.IncludeSubdirectories = true;
			}
		}
	}
}