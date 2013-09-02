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

		private ProjectFile _fileFound;
		private ProjectFolder _mainFolder;
		private readonly IAssemblerService _assemblerService;

		public WabbitcodeProject(IAssemblerService assemblerService)
		{
			IncludeDirs = new List<string>();
			_assemblerService = assemblerService;
			BuildSystem = new BuildSystem(_assemblerService, this);
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

		public IBuildSystem BuildSystem { get; private set; }

		public IList<string> IncludeDirs { get; private set; }

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

		public bool ContainsFile(string fullPath)
		{
			_fileFound = null;
			return RecurseSearchFolders(MainFolder, Path.GetFileName(fullPath));
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

		private void BuildXMLFile()
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
			BuildSystem.WriteXML(writer);
			writer.WriteEndElement();
			writer.Flush();
			writer.Close();
		}

		public void CreateNewProject(string projectFile, string projectName)
		{
			ProjectFolder folder = new ProjectFolder(null, projectName);
			_mainFolder = folder;
			ProjectName = projectName;
			ProjectFile = projectFile;
			ProjectDirectory = Path.GetDirectoryName(projectFile);

			BuildSystem = new BuildSystem(_assemblerService, this);
			BuildSystem.CreateDefaultConfigs();
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

		public void OpenProject(string projectFile)
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
					_mainFolder = new ProjectFolder(null, reader.GetAttribute("Name"));
					RecurseReadFolders(reader, ref _mainFolder);
					BuildSystem.ReadXML(reader);
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
				switch (reader.Name)
				{
					case "Folder":
					{
						ProjectFolder subFolder = new ProjectFolder(folder, reader.GetAttribute("Name"));
						folder.AddFolder(subFolder);
						RecurseReadFolders(reader, ref subFolder);
					}
						break;
					case "File":
					{
						ProjectFile file = new ProjectFile(folder, reader.GetAttribute("Path"), ProjectDirectory);
						folder.AddFile(file);
					}
						break;
					default:
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