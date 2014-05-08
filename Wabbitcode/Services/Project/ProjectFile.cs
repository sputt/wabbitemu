using System.IO;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Project
{
    public class ProjectFile
    {
	    private FilePath _filePath;
	    private readonly FilePath _projectDir;
        private ProjectFolder _folder;
        private readonly ProjectFolder _parentFolder;

        public ProjectFile(ProjectFolder projectFolder, FilePath fullPath, FilePath projectDir)
        {
            _parentFolder = projectFolder;
            _filePath = fullPath;
	        _projectDir = projectDir;
        }

	    public string FileFoldings { get; set; }

	    public FilePath FileFullPath
        {
            get
            {
	            return Path.IsPathRooted(_filePath) ? 
                    _filePath :
                    _projectDir.GetAbsolutePath(_filePath);
            }

		    set
            {
                _filePath = value;
            }
        }

        public FilePath FileRelativePath
        {
            get
            {
				return Path.IsPathRooted(_filePath) ?
                    new FilePath(FileOperations.GetRelativePath(_projectDir, _filePath)) : 
                    _filePath;
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