namespace Revsoft.Wabbitcode.Services.Project
{
    using Revsoft.Wabbitcode.Classes;
    using Revsoft.Wabbitcode.Properties;
    using Revsoft.Wabbitcode.Services;
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Text;
    using System.Windows.Forms;
    using System.Xml;

    public class WabbitcodeProject : IProject
    {
        private const string ProjectFileVersion = "1.0";

        private BuildSystem buildSystem = new BuildSystem(false);
        private ProjectFile fileFound;
        private List<string> includeDir = new List<string>();
        private List<string> labelOutputs = new List<string>();
        private List<string> listOutputs = new List<string>();
        private ProjectFolder mainFolder;
        private bool needsSave = false;
        private string projectDirectory;
        private string projectFile;
        private string projectName;
        private List<string> projectOutputs = new List<string>();

        public WabbitcodeProject()
        {
        }

        public WabbitcodeProject(string projectFile)
        {
            ProjectService.IsInternal = false;
            this.projectFile = projectFile;
            this.projectDirectory = Path.GetDirectoryName(projectFile);

            Settings.Default.includeDir = "";
        }

        #region Public Members
        public IBuildSystem BuildSystem
        {
            get
            {
                return this.buildSystem;
            }
        }

        public List<string> IncludeDir
        {
            get
            {
                return this.includeDir;
            }
            set
            {
                this.includeDir = value;
            }
        }

        public List<string> LabelOutputs
        {
            get
            {
                return this.labelOutputs;
            }
        }

        public List<string> ListOutputs
        {
            get
            {
                return this.listOutputs;
            }
        }

        public ProjectFolder MainFolder
        {
            get
            {
                return this.mainFolder;
            }
            set
            {
                this.mainFolder = value;
            }
        }

        public bool NeedsSave
        {
            get
            {
                return this.needsSave;
            }
            set
            {
                this.needsSave = value;
            }
        }

        public string ProjectDirectory
        {
            get
            {
                return this.projectDirectory;
            }
        }

        public string ProjectFile
        {
            get
            {
                return this.projectFile;
            }
        }

        public string ProjectName
        {
            get
            {
                return this.projectName;
            }
            set
            {
                this.projectName = value;
            }
        }

        public List<string> ProjectOutputs
        {
            get
            {
                return this.projectOutputs;
            }
        }
        #endregion

        public ProjectFile AddFile(ProjectFolder parentFolder, string fullPath)
        {
            ProjectFile file = new ProjectFile(this, fullPath);
            parentFolder.AddFile(file);
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
            this.fileFound = null;
            return this.RecurseSearchFolders(this.MainFolder, Path.GetFileName(fullPath));
        }

        public void DeleteFile(ProjectFolder parentDir, ProjectFile file)
        {
            file.Remove();
        }

        public void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir)
        {
            parentDir.Folders.Remove(dir);
        }

        public ProjectFile FindFile(string fullPath)
        {
            if (this.fileFound != null && string.Equals(this.fileFound.FileFullPath, fullPath, StringComparison.OrdinalIgnoreCase))
            {
                return this.fileFound;
            }
            this.fileFound = null;
            if (this.ContainsFile(fullPath))
            {
                return this.fileFound;
            }
            return null;
        }

        internal void BuildXMLFile()
        {
            XmlTextWriter writer = new XmlTextWriter(this.projectFile, Encoding.Unicode);
            writer.Formatting = Formatting.Indented;
            writer.WriteStartDocument();
            writer.WriteComment("Wabbitcode Config File");
            writer.WriteStartElement("WabbitcodeProject");
            writer.WriteAttributeString("Version", ProjectFileVersion);
            writer.WriteAttributeString("Name", this.projectName);
            this.RecurseWriteFolders(writer, this.mainFolder);
            this.buildSystem.WriteXML(writer);
            writer.WriteEndElement();
            writer.Flush();
            writer.Close();
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

        internal int GetOutputType()
        {
            foreach (IBuildStep step in this.buildSystem.CurrentConfig.Steps)
            {
                if (step is InternalBuildStep)
                {
                    string outputFile = ((InternalBuildStep)step).OutputFile;
                    if (Path.GetExtension(outputFile) == ".8xk")
                    {
                        return 5;
                    }
                }
            }

            // otherwise we assume its a program
            return 4;
        }

        internal List<ProjectFile> GetProjectFiles()
        {
            List<ProjectFile> files = new List<ProjectFile>();
            this.RecurseAddFiles(ref files, this.mainFolder);
            return files;
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
                    this.projectName = reader.GetAttribute("Name");
                    reader.MoveToNextElement();
                    if (reader.Name != "Folder")
                    {
                        throw new ArgumentException("Invalid XML Format");
                    }
                    this.mainFolder = new ProjectFolder(this, reader.GetAttribute("Name"));
                    this.RecurseReadFolders(reader, ref this.mainFolder);
                    this.buildSystem.ReadXML(reader);
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

        private void RecurseAddFiles(ref List<ProjectFile> files, ProjectFolder folder)
        {
            foreach (ProjectFolder subFolder in folder.Folders)
            {
                this.RecurseAddFiles(ref files, subFolder);
            }
            foreach (ProjectFile file in folder.Files)
            {
                files.Add(file);
            }
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
                    this.RecurseReadFolders(reader, ref subFolder);
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
            if (this.fileFound == null)
            {
                this.fileFound = folder.FindFile(file);
            }
            if (this.fileFound != null)
            {
                return true;
            }
            bool returnVal = false;
            foreach (ProjectFolder subFolder in folder.Folders)
            {
                returnVal |= this.RecurseSearchFolders(subFolder, file);
            }
            if (this.fileFound != null)
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
                this.RecurseWriteFolders(writer, subFolder);
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
    }
}