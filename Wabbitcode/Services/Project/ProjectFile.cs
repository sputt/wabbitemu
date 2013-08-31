using System.IO;
using Revsoft.Wabbitcode.Extensions;

namespace Revsoft.Wabbitcode.Services.Project
{
    public class ProjectFile
    {
	    private string _filePath;
	    private readonly string _projectDir;
        private ProjectFolder _folder;
        private readonly ProjectFolder _parentFolder;

        public ProjectFile(ProjectFolder projectFolder, string fullPath, string projectDir)
        {
            _parentFolder = projectFolder;
            _filePath = fullPath;
	        _projectDir = projectDir;
        }

	    public string FileFoldings { get; set; }

	    public string FileFullPath
        {
            get
            {
	            return Path.IsPathRooted(_filePath) ? _filePath : FileOperations.GetAbsolutePath(_projectDir, _filePath);
            }

		    set
            {
                _filePath = value;
            }
        }

        public string FileRelativePath
        {
            get
            {
				return Path.IsPathRooted(_filePath) ? FileOperations.GetRelativePath(_projectDir, _filePath) : _filePath;
            }
	        set
            {
                _filePath = value;
            }
        }

        public ProjectFolder Folder
        {
            get
            {
                return _folder;
            }
            set
            {
                _folder = value;
            }
        }

        public ProjectFolder ParentFolder
        {
            get
            {
                return _parentFolder;
            }
        }

        public override string ToString()
        {
            return _filePath;
        }

        internal void Remove()
        {
            _folder.Files.Remove(this);
            _folder = null;
        }
    }
}