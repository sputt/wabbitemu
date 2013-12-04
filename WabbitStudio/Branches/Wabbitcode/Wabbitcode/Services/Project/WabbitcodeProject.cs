using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Extensions;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using Revsoft.Wabbitcode.Properties;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class WabbitcodeProject : IProject
	{
		private const string ProjectFileVersion = "1.0";

		private ProjectFile _fileFound;
		private ProjectFolder _mainFolder;
	    private readonly FileSystemWatcher _watcher;

	    public WabbitcodeProject()
		{
			IncludeDirs = new List<string>();
		    BuildSystem = new BuildSystem(this);
			_watcher = new FileSystemWatcher();
            _watcher.Changed += Watcher_OnChanged;
            _watcher.Renamed += Watcher_OnRenamed;
		}

	    public WabbitcodeProject(string projectFile)
			: this()
		{
			ProjectFile = projectFile;
			ProjectDirectory = Path.GetDirectoryName(projectFile);
            SetupWatcher();
		}

		#region Public Members

	    public event EventHandler<FileModifiedEventArgs> FileModifiedExternally;
        
        public bool IsInternal { get; set; }

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

		public bool ContainsFile(string file)
		{
			_fileFound = null;
			return RecurseSearchFolders(MainFolder, Path.GetFileName(file));
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
		    using (XmlTextWriter writer = new XmlTextWriter(ProjectFile, Encoding.Unicode)
		    {
		        Formatting = Formatting.Indented
		    })
		    {
		        writer.WriteStartDocument();
		        writer.WriteComment("Wabbitcode Config File");
		        writer.WriteStartElement("WabbitcodeProject");
		        writer.WriteAttributeString("Version", ProjectFileVersion);
		        writer.WriteAttributeString("Name", ProjectName);
		        RecurseWriteFolders(writer, _mainFolder);
		        BuildSystem.WriteXML(writer);
		        writer.WriteEndElement();
		        writer.Flush();
		    }
		}

		public void CreateNewProject(string projectFile, string projectName)
		{
			ProjectFolder folder = new ProjectFolder(null, projectName);
			_mainFolder = folder;
			ProjectName = projectName;
			ProjectFile = projectFile;
			ProjectDirectory = Path.GetDirectoryName(projectFile);

			BuildSystem = new BuildSystem(this);
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

	    public void EnableFileWatcher(bool enabled)
	    {
	        if (!string.IsNullOrEmpty(_watcher.Path))
	        {
	            _watcher.EnableRaisingEvents = enabled;
	        }
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
					if (formatVersion != ProjectFileVersion)
					{
					    throw new InvalidProjectVersionException();
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

			SetupWatcher();
		}

		private static void RecurseAddFiles(ref List<ProjectFile> files, ProjectFolder folder)
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

		private static void RecurseWriteFolders(XmlTextWriter writer, ProjectFolder folder)
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

        /// <summary>
        /// Attempts to find an existing file in the set of include dirs,
        /// given the relative path.
        /// </summary>
        /// <param name="relativePath">The relative file path to search for</param>
        /// <returns>The absolute path string. Null if an existing file cannot be found.</returns>
	    public string GetFilePathFromRelativePath(string relativePath)
	    {
            IEnumerable<string> includeDirs = IsInternal ?
                Settings.Default.IncludeDirs.Cast<string>() :
                IncludeDirs;

            foreach (string dir in includeDirs.Where(dir => File.Exists(Path.Combine(dir, relativePath))))
            {
                return Path.Combine(dir, relativePath);
            }

            if (IsInternal)
            {
                return null;
            }

            return File.Exists(Path.Combine(ProjectDirectory, relativePath)) ?
                Path.Combine(ProjectDirectory, relativePath) :
                null;
        }

        #region FileSystemWatcher

        private void SetupWatcher()
        {
            if (string.IsNullOrEmpty(ProjectDirectory))
            {
                return;
            }

            _watcher.Path = ProjectDirectory;
            _watcher.EnableRaisingEvents = true;
            _watcher.IncludeSubdirectories = true;
        }

        private void Watcher_OnRenamed(object sender, RenamedEventArgs e)
        {
            if (e.OldFullPath != ProjectDirectory)
            {
                return;
            }

            if (MessageBox.Show("Project Folder was renamed, would you like to rename the project?",
                    "Rename project",
                    MessageBoxButtons.YesNo,
                    MessageBoxIcon.Information) == DialogResult.Yes)
            {
                ProjectName = Path.GetFileNameWithoutExtension(e.FullPath);
            }
        }

        private void Watcher_OnChanged(object sender, FileSystemEventArgs e)
        {
            if (FileModifiedExternally == null)
            {
                return;
            }

            var projectFile = FindFile(e.FullPath);
            if (projectFile != null)
            {
                FileModifiedExternally(this, new FileModifiedEventArgs(projectFile));
            }
        }

        #endregion
    }
}