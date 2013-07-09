namespace Revsoft.Wabbitcode.Services.Project
{
    using Revsoft.Wabbitcode;
    using System.IO;

    public class ProjectFile
    {
        private string fileFoldings;
        private string filePath;
        private ProjectFolder folder;
        private IProject parent;

        public ProjectFile(IProject project, string fullPath)
        {
            this.parent = project;
            this.filePath = fullPath;
        }

        public string FileFoldings
        {
            get
            {
                return this.fileFoldings;
            }
            set
            {
                this.fileFoldings = value;
            }
        }

        public string FileFullPath
        {
            get
            {
                if (Path.IsPathRooted(this.filePath))
                {
                    return this.filePath;
                }
                else
                {
                    return FileOperations.GetAbsolutePath(this.parent.ProjectDirectory, this.filePath);
                }
            }

            set
            {
                this.filePath = value;
            }
        }

        public string FileRelativePath
        {
            get
            {
                if (Path.IsPathRooted(this.filePath))
                {
                    return FileOperations.GetRelativePath(this.parent.ProjectDirectory, this.filePath);
                }
                else
                {
                    return this.filePath;
                }
            }

            set
            {
                this.filePath = value;
            }
        }

        public ProjectFolder Folder
        {
            get
            {
                return this.folder;
            }
            set
            {
                this.folder = value;
            }
        }

        public IProject Parent
        {
            get
            {
                return this.parent;
            }
        }

        public override string ToString()
        {
            return this.filePath;
        }

        internal void Remove()
        {
            this.folder.Files.Remove(this);
            this.folder = null;
        }
    }
}