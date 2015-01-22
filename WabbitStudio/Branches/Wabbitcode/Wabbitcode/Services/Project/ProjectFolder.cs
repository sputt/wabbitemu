using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Revsoft.Wabbitcode.Services.Project
{
    public class ProjectFolder
    {
        private string _name;
        public ProjectFolder(ProjectFolder parent, string folderName)
        {
            Parent = parent;
            Name = folderName;
            Files = new List<ProjectFile>();
            Folders = new List<ProjectFolder>();
        }

        public List<ProjectFile> Files { get; private set; }

        public List<ProjectFolder> Folders { get; private set; }

        public string Name
        {
            get { return _name; }
            set
            {
                if (string.IsNullOrEmpty(value))
                {
                    throw new ArgumentNullException();
                }

                _name = value;
            }
        }

        public ProjectFolder Parent { get; private set; }

        private ProjectFolder ParentFolder { get; set; }

        public override string ToString()
        {
            return Name;
        }

        internal void AddFile(ProjectFile file)
        {
            Files.Add(file);
            file.Folder = this;
        }

        internal void AddFolder(ProjectFolder subFolder)
        {
            Folders.Add(subFolder);
            subFolder.ParentFolder = this;
        }

        /// <summary>
        /// Returns if the filename is contained in the list of files
        /// </summary>
        /// <param name="file">The file name to search for, not full path</param>
        /// <returns>File found otherwise null</returns>
        internal ProjectFile FindFile(string file)
        {
            return Files.FirstOrDefault(subFile => string.Equals(Path.GetFileName(subFile.FileFullPath), file, StringComparison.OrdinalIgnoreCase));
        }

        /// <summary>
        /// Searches for a folder in subfolders. Does not recurse.
        /// </summary>
        /// <param name="folder"></param>
        /// <returns>Returns folder found, otherwise null</returns>
        internal ProjectFolder FindFolder(string folder)
        {
            return Folders.FirstOrDefault(subFolder => subFolder.Name == folder);
        }

        public void DeleteFolder(ProjectFolder folder)
        {
            Folders.Remove(folder);
        }

        public void DeleteFile(ProjectFile file)
        {
            Files.Remove(file);
        }
    }
}