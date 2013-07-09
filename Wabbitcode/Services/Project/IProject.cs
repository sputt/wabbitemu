namespace Revsoft.Wabbitcode.Services.Project
{
    using System.Collections.Generic;

    public interface IProject
    {
        IBuildSystem BuildSystem
        {
            get;
        }

        List<string> IncludeDir
        {
            get;
        }

        ProjectFolder MainFolder
        {
            get;
        }

        bool NeedsSave
        {
            get;
            set;
        }

        string ProjectDirectory
        {
            get;
        }

        string ProjectFile
        {
            get;
        }

        string ProjectName
        {
            get;
            set;
        }

        List<string> ProjectOutputs
        {
            get;
        }

        List<string> LabelOutputs
        {
            get;
        }

        List<string> ListOutputs
        {
            get;
        }

        ProjectFile AddFile(ProjectFolder parentFolder, string fullPath);

        ProjectFolder AddFolder(string dirName, ProjectFolder parentFolder);

        void DeleteFile(ProjectFolder parentDir, ProjectFile file);

        void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir);
    }
}