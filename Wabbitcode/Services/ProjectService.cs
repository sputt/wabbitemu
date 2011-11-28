using System;
using System.Collections.Generic;
using System.IO;
using Revsoft.Wabbitcode.Services.Project.Interface;
using Revsoft.Wabbitcode.Interface.Services;

namespace Revsoft.Wabbitcode.Services
{
	[ServiceDependency("PathsService")]
	public class ProjectService : IProjectService
	{
		private IPathsService pathsService;
		public IParserService ParserService { get; set; }

		public List<IProject> OpenProjects { get; private set; }

		IProject curProj;
		public IProject CurrentProject
		{
			get { return curProj; }
			set
			{
				curProj = value;
                if (CurrentProjectChanged != null)
                {
                    CurrentProjectChanged(value, EventArgs.Empty);
                }
			}
		}

		public void InitService(params Object[] objects)
		{
			OpenProjects = new List<IProject>();
			CurrentProject = new Project.InternalProject();
		}

        public void DestroyService()
        {
            OpenProjects = null;
            CurrentProject = null;
        }

		public delegate void ChangingHandler(object sender, EventArgs e);
		public event ChangingHandler CurrentProjectChanged;

		public delegate void OpenedHandler(object sender, EventArgs e);
		public event OpenedHandler ProjectOpened;

		public void OpenProject(Stream stream, string fileName)
		{
			IProject project = new Project.Project(stream, fileName);
			CurrentProject = project;
			OpenProjects.Add(project);
			ProjectOpened(project, EventArgs.Empty);
		}
	}
}
