using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode.Services
{
    public static class ProjectService
    {
        public static List<ProjectClass> OpenProjects { get; private set; }

        static ProjectClass curProj;
        public static ProjectClass CurrentProject
        {
            get { return curProj; }
            set
            {
                curProj = value;
                if (CurrentProjectChanged != null)
                    CurrentProjectChanged(value, EventArgs.Empty);
            }
        }

        public static void InitProjects()
        {
            OpenProjects = new List<ProjectClass>();
            CurrentProject = new ProjectClass() { IsInternal = true, };
        }

        public delegate void ChangingHandler(object sender, EventArgs e);
        public static event ChangingHandler CurrentProjectChanged;

        internal static void OpenProject(string fileName)
        {
            ProjectClass project = new ProjectClass();
            project.OpenProject(fileName);
            CurrentProject = project;
            OpenProjects.Add(project);
        }
    }
}
