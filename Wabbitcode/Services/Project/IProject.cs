using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Project
{
    public interface IProject
    {
        event EventHandler<FileModifiedEventArgs> FileModifiedExternally;

        IBuildSystem BuildSystem { get; }

        IList<FilePath> IncludeDirs { get; }

        ProjectFolder MainFolder { get; }

        bool NeedsSave { get; }

        FilePath ProjectDirectory { get; }

        FilePath ProjectFile { get; }

        string ProjectName { get; set; }

        bool IsInternal { get; }

        IList<DebuggingStructure> DebuggingStructures { get; }

        IEnumerable<ProjectFile> GetProjectFiles();
        bool ContainsFile(FilePath file);
        ProjectFile FindFile(FilePath fullPath);
        FilePath GetFilePathFromRelativePath(string relativePath);


        void CreateNewProject(FilePath projectFile, string projectName);
        void OpenProject(FilePath projectFile);
        void SaveProject();

        void EnableFileWatcher(bool enabled);
    }

    public class FileModifiedEventArgs : EventArgs
    {
        public ProjectFile File { get; set; }

        public FileModifiedEventArgs(ProjectFile file)
        {
            File = file;
        }
    }
}