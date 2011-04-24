using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Services.Project;
using System.IO;

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
            if (!File.Exists(WabbitcodePaths.TemplatesConfig))
            {
                string templateXML = ResourceService.GetResource("Templates.xml");
                var sw = new StreamWriter(WabbitcodePaths.TemplatesConfig);
                sw.Write(templateXML);
                sw.Flush();
                sw.Close();
            }
        }

        public delegate void ChangingHandler(object sender, EventArgs e);
        public static event ChangingHandler CurrentProjectChanged;

        public delegate void OpenedHandler(object sender, EventArgs e);
        public static event OpenedHandler ProjectOpened;

        internal static void OpenProject(string fileName)
        {
            ProjectClass project = ProjectClass.OpenProject(fileName);
            CurrentProject = project;
            OpenProjects.Add(project);
            ProjectOpened(project, EventArgs.Empty);
        }
    }
}
