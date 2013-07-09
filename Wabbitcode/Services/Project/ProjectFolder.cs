namespace Revsoft.Wabbitcode.Services.Project
{
    using System;
    using System.Collections.Generic;
    using System.IO;

    public class ProjectFolder
    {
        private List<ProjectFile> files;
        private List<ProjectFolder> folders;
        private string name;
        private IProject parent;
        private ProjectFolder parentFolder;

        public ProjectFolder(IProject parent, string folderName)
        {
            this.parent = parent;
            this.name = folderName;
            this.files = new List<ProjectFile>();
            this.folders = new List<ProjectFolder>();
        }

        public List<ProjectFile> Files
        {
            get
            {
                return this.files;
            }
        }

        public List<ProjectFolder> Folders
        {
            get
            {
                return this.folders;
            }
        }

        public string Name
        {
            get
            {
                return this.name;
            }
            set
            {
                this.name = value;
            }
        }

        public IProject Parent
        {
            get
            {
                return this.parent;
            }
        }

        public ProjectFolder ParentFolder
        {
            get
            {
                return this.parentFolder;
            }
            set
            {
                this.parentFolder = value;
            }
        }

        public override string ToString()
        {
            return this.name;
        }

        internal void AddFile(ProjectFile file)
        {
            this.files.Add(file);
            file.Folder = this;
            this.parent.NeedsSave = true;
        }

        internal void AddFolder(ProjectFolder subFolder)
        {
            this.folders.Add(subFolder);
            subFolder.ParentFolder = this;
            this.parent.NeedsSave = true;
        }

        /// <summary>
        /// Returns if the filename is contained in the list of files
        /// </summary>
        /// <param name="file">The file name to search for, not full path</param>
        /// <returns>File found otherwise null</returns>
        internal ProjectFile FindFile(string file)
        {
            foreach (ProjectFile subFile in this.files)
            {
                if (string.Equals(Path.GetFileName(subFile.FileFullPath), file, StringComparison.OrdinalIgnoreCase))
                {
                    return subFile;
                }
            }
            return null;
        }

        /// <summary>
        /// Searches for a folder in subfolders.
        /// </summary>
        /// <param name="folder"></param>
        /// <returns>Returns folder found, otherwise null</returns>
        internal ProjectFolder FindFolder(string folder)
        {
            foreach (ProjectFolder subFolder in this.folders)
            {
                if (subFolder.name == folder)
                {
                    return subFolder;
                }
            }
            return null;
        }

        internal void Remove()
        {
            this.parentFolder.Folders.Remove(this);
            this.parentFolder = null;
            this.parent.NeedsSave = true;
        }
    }
}