using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Extensions;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Xml;
using Revsoft.Wabbitcode.GUI.DockingWindows.Tracking;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Project
{
    public class WabbitcodeProject : IProject, IDisposable
    {
        private const string ProjectFileVersion = "1.0";

        private readonly FileSystemWatcher _watcher;
        private ProjectFile _fileFound;
        private ProjectFolder _mainFolder;
        private DebuggingStructureList _debuggingStructure;

        public WabbitcodeProject()
        {
            IncludeDirs = new List<FilePath>();
            BuildSystem = new BuildSystem(this);
            _watcher = new FileSystemWatcher();
            _watcher.Changed += Watcher_OnChanged;
            _watcher.Renamed += Watcher_OnRenamed;
        }

        public WabbitcodeProject(FilePath projectFile)
            : this()
        {
            ProjectFile = projectFile;
            ProjectDirectory = projectFile.GetDirectoryName();
            SetupWatcher();
        }

        #region Public Members

        public event EventHandler<FileModifiedEventArgs> FileModifiedExternally;

        public bool IsInternal { get; set; }

        public IBuildSystem BuildSystem { get; private set; }

        public IList<FilePath> IncludeDirs { get; private set; }

        public bool NeedsSave { get; private set; }

        public FilePath ProjectDirectory { get; private set; }

        public FilePath ProjectFile { get; private set; }

        public string ProjectName { get; set; }

        public ProjectFolder MainFolder
        {
            get { return _mainFolder; }
        }

        #endregion

        public bool ContainsFile(FilePath file)
        {
            _fileFound = null;
            return RecurseSearchFolders(MainFolder, Path.GetFileName(file));
        }

        public ProjectFile FindFile(FilePath fullPath)
        {
            if (_fileFound != null && string.Equals(_fileFound.FileFullPath, fullPath, StringComparison.OrdinalIgnoreCase))
            {
                return _fileFound;
            }
            _fileFound = null;
            return ContainsFile(fullPath) ? _fileFound : null;
        }

        private void BuildXmlFile()
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
                _debuggingStructure.WriteXml(writer);
                BuildSystem.WriteXML(writer);
                writer.WriteEndElement();
                writer.Flush();
            }
        }

        public void CreateNewProject(FilePath projectFile, string projectName)
        {
            ProjectFolder folder = new ProjectFolder(null, projectName);
            _mainFolder = folder;
            ProjectName = projectName;
            ProjectFile = projectFile;
            ProjectDirectory = projectFile.GetDirectoryName();

            BuildSystem = new BuildSystem(this);
            BuildSystem.CreateDefaultConfigs();
        }

        public IEnumerable<ProjectFile> GetProjectFiles()
        {
            var files = new List<ProjectFile>();
            RecurseAddFiles(ref files, _mainFolder);
            return files;
        }

        public void SaveProject()
        {
            BuildXmlFile();
            NeedsSave = false;
        }

        public void EnableFileWatcher(bool enabled)
        {
            if (!string.IsNullOrEmpty(_watcher.Path))
            {
                _watcher.EnableRaisingEvents = enabled;
            }
        }

        public void OpenProject(FilePath projectFile)
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
                    reader.MoveToNextElement();

                    _debuggingStructure = DebuggingStructureList.FromXml(reader);
                    var displayManager = VariableDisplayManager.Instance;
                    foreach (var debugEnum in _debuggingStructure.DebugEnums)
                    {
                        var enumController = new EnumVariableDisplayController(new ExpressionEvaluator(), debugEnum);
                        displayManager.RegisterVariableDisplayController(enumController);
                    }
                    foreach (var structure in _debuggingStructure.DebugStructs)
                    {
                        displayManager.RegisterVariableDisplayController(new CompositeVariableDisplayController(structure));
                    }

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
                        FilePath path = new FilePath(reader.GetAttribute("Path"));
                        ProjectFile file = new ProjectFile(folder, path, ProjectDirectory);
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
        public FilePath GetFilePathFromRelativePath(string relativePath)
        {
            var includeDirs = IsInternal ?
                Settings.Default.IncludeDirs.Cast<string>().Select(path => new FilePath(path)) :
                IncludeDirs;

            foreach (FilePath dir in includeDirs.Where(dir => File.Exists(Path.Combine(dir, relativePath))))
            {
                return dir.Combine(relativePath);
            }

            if (IsInternal)
            {
                return null;
            }

            return File.Exists(ProjectDirectory.Combine(relativePath)) ?
                ProjectDirectory.Combine(relativePath) :
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

            var projectFile = FindFile(new FilePath(e.FullPath));
            if (projectFile != null)
            {
                FileModifiedExternally(this, new FileModifiedEventArgs(projectFile));
            }
        }

        #endregion

        public void Dispose()
        {
            Dispose(true);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                _watcher.Dispose();
            }
        }
    }

    internal class DebuggingStructureList
    {
        public List<DebuggingStructure> DebugStructs { get; private set; }

        public List<DebuggingEnum> DebugEnums { get; private set; }

        public DebuggingStructureList()
        {
            DebugStructs = new List<DebuggingStructure>();
            DebugEnums = new List<DebuggingEnum>();
        }

        public void WriteXml(XmlTextWriter writer)
        {
            writer.WriteStartElement("DebugStructs");
            foreach (var debugEnum in DebugEnums)
            {
                writer.WriteStartElement("Enum");
                writer.WriteAttributeString("Name", debugEnum.Name);
                foreach (var enumValue in debugEnum.EnumMapping)
                {
                    writer.WriteStartElement("EnumMask");
                    writer.WriteAttributeString("Mask", enumValue.Key.ToString());

                    var maskList = enumValue.Value;
                    foreach (var value in maskList)
                    {
                        writer.WriteStartElement("EnumValue");
                        writer.WriteString(value);
                        writer.WriteEndElement();
                    }

                    writer.WriteEndElement();
                }

                writer.WriteEndElement();
            }

            foreach (var structure in DebugStructs)
            {
                writer.WriteStartElement("Struct");
                writer.WriteAttributeString("Name", structure.Name);
                foreach (var prop in structure.Properties)
                {
                    writer.WriteStartElement("Prop");
                    writer.WriteAttributeString("Name", prop.Name);
                    writer.WriteAttributeString("Size", prop.Size.ToString());
                    writer.WriteAttributeString("Type", prop.Type);
                    writer.WriteEndElement();
                }

                writer.WriteEndElement();
            }

            writer.WriteEndElement();
        }

        public static DebuggingStructureList FromXml(XmlTextReader reader)
        {
            DebuggingStructureList list = new DebuggingStructureList();
            if (reader.Name != "DebugStructs")
            {
                return list;
            }

            if (reader.IsEmptyElement)
            {
                reader.MoveToNextElement();
                return list;
            }

            reader.MoveToNextElement();
            while (reader.Name == "Enum")
            {
                var name = reader.GetAttribute("Name");
                var enumValues = new Dictionary<byte, List<string>>();
                if (!reader.MoveToNextElement())
                {
                    break;
                }

                while (reader.Name == "EnumMask")
                {
                    var maskList = new List<string>();
                    byte maskValue;
                    if (!byte.TryParse(reader.GetAttribute("Mask"), out maskValue))
                    {
                        continue;
                    }

                    while (reader.MoveToNextElement())
                    {
                        if (reader.Name != "EnumValue")
                        {
                            break;
                        }

                        maskList.Add(reader.ReadString());
                    }

                    
                    enumValues.Add(maskValue, maskList);
                }

                var debugEnum = new DebuggingEnum(name, enumValues);
                list.DebugEnums.Add(debugEnum);
            }

            while (reader.Name == "Struct")
            {
                var name = reader.GetAttribute("Name");
                var structure = new DebuggingStructure {Name = name};
                while (reader.MoveToNextElement())
                {
                    if (reader.Name != "Prop")
                    {
                        break;
                    }

                    var model = new TreeStructureModel
                    {
                        Name = reader.GetAttribute("Name"),
                        Size = Convert.ToInt32(reader.GetAttribute("Size")),
                        Type = reader.GetAttribute("Type")
                    };

                    structure.Properties.Add(model);
                }

                list.DebugStructs.Add(structure);
            }

            return list;
        }
    }

    internal class DebuggingEnum
    {
        public string Name { get; private set; }
        public Dictionary<byte, List<string>> EnumMapping { get; private set; }

        public DebuggingEnum(string enumName, Dictionary<byte, List<string>> enumMapping)
        {
            Name = enumName;
            EnumMapping = enumMapping;
        }
    }
}