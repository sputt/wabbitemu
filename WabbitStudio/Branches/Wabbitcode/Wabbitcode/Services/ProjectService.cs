using System;
using System.Collections.Concurrent;
using System.Threading.Tasks;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;
using System.Collections.Generic;
using System.Linq;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public class ProjectService : IProjectService
    {
        public const string ProjectExtension = ".wcodeproj";

        #region Events

        public event EventHandler ProjectOpened;
        public event EventHandler ProjectClosed;
        public event EventHandler ProjectFileAdded;
        public event EventHandler ProjectFolderAdded;
        public event EventHandler ProjectFileRemoved;
        public event EventHandler ProjectFolderRemoved;

        #endregion

        private readonly List<ParserInformation> _parseInfo = new List<ParserInformation>();
        private readonly IParserService _parserService;
        private readonly IStatusBarService _statusBarService;

        private IList<ParserInformation> ParserInfomInformation
        {
            get { return _parseInfo; }
        }

        public IProject Project { get; private set; }

        public ProjectService(IParserService parserService, IStatusBarService statusBarService,
            IFileTypeMethodFactory fileTypeMethodFactory)
        {
            _parserService = parserService;
            _statusBarService = statusBarService;
            fileTypeMethodFactory.RegisterFileType(ProjectExtension, OpenProject);
        }

        public bool OpenProject(FilePath fileName)
        {
            Project = new WabbitcodeProject(fileName);
            Project.OpenProject(fileName);

            _parseInfo.Clear();
            Task.Factory.StartNew(() =>
            {
                _statusBarService.ShowProgressBar(true);
                ParseFiles(Project.MainFolder, 100);
                _statusBarService.ShowProgressBar(false);
            });

            if (ProjectOpened != null)
            {
                ProjectOpened(this, EventArgs.Empty);
            }

            return true;
        }

        public ProjectFile AddFile(ProjectFolder parent, FilePath fullPath)
        {
            ProjectFile file = new ProjectFile(parent, fullPath, Project.ProjectDirectory);
            parent.AddFile(file);
            if (ProjectFileAdded != null)
            {
                ProjectFileAdded(this, EventArgs.Empty);
            }

            return file;
        }

        public ProjectFolder AddFolder(string folderName, ProjectFolder parentDir)
        {
            ProjectFolder folder = new ProjectFolder(parentDir, folderName);
            parentDir.AddFolder(folder);
            if (ProjectFolderAdded != null)
            {
                ProjectFolderAdded(this, EventArgs.Empty);
            }

            return folder;
        }

        public void CloseProject()
        {
            if (Project.IsInternal)
            {
                Project = null;
                return;
            }


            if (ProjectClosed != null)
            {
                ProjectClosed(this, EventArgs.Empty);
            }

            CreateInternalProject();
        }

        public bool ContainsFile(FilePath file)
        {
            return file != null && Project.ContainsFile(file);
        }

        public IProject CreateInternalProject()
        {
            Project = new WabbitcodeProject
            {
                IsInternal = true
            };

            if (ProjectOpened != null)
            {
                ProjectOpened(this, EventArgs.Empty);
            }

            return Project;
        }

        public IProject CreateNewProject(FilePath projectFile, string projectName)
        {
            Project = new WabbitcodeProject();
            Project.CreateNewProject(projectFile, projectName);

            if (ProjectOpened != null)
            {
                ProjectOpened(this, EventArgs.Empty);
            }

            return Project;
        }

        public void DeleteFile(FilePath fullPath)
        {
            ProjectFile file = Project.FindFile(fullPath);
            DeleteFile(file.Folder, file);
        }

        public void DeleteFile(ProjectFolder parentDir, ProjectFile file)
        {
            RemoveParseData(file.FileFullPath);
            parentDir.DeleteFile(file);

            if (ProjectFileRemoved != null)
            {
                ProjectFileRemoved(this, EventArgs.Empty);
            }
        }

        public void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir)
        {
            parentDir.DeleteFolder(dir);
            if (ProjectFolderRemoved != null)
            {
                ProjectFolderRemoved(this, EventArgs.Empty);
            }
        }

        public void RemoveParseData(FilePath fullPath)
        {
            ParserInformation replaceMe = GetParseInfo(fullPath);
            if (replaceMe != null)
            {
                ParserInfomInformation.Remove(replaceMe);
            }
        }

        public ParserInformation GetParseInfo(FilePath file)
        {
            lock (_parseInfo)
            {
                foreach (var info in _parseInfo.Where(info => string.Equals(info.SourceFile, file)))
                {
                    return info;
                }
            }

            return null;
        }

        public void SaveProject()
        {
            Project.SaveProject();
        }

        private void ParseFiles(ProjectFolder folder, int incrementValue)
        {
            double folderInc = (double) incrementValue / (folder.Folders.Count + 1);
            foreach (ProjectFolder subFolder in folder.Folders)
            {
                ParseFiles(subFolder, (int) folderInc);
            }

            double fileInc = folderInc / folder.Files.Count;
            foreach (ProjectFile file in folder.Files.ToArray())
            {
                _parserService.ParseFile(0, file.FileFullPath);
                _statusBarService.IncrementProgressBarProgress((int) fileInc);
            }
        }

        public IEnumerable<List<Reference>> FindAllReferences(string refString)
        {
            var refsList = new ConcurrentBag<List<Reference>>();
            var files = Project.GetProjectFiles();

            Task.WaitAll(files.Select(file => Project.ProjectDirectory.Combine(file.FileFullPath))
                .Select(filePath => Task.Factory.StartNew(() =>
                {
                    var refs = _parserService.FindAllReferencesInFile(filePath, refString);
                    if (refs.Any())
                    {
                        refsList.Add(refs);
                    }
                })).ToArray());

            return refsList;
        }
    }
}