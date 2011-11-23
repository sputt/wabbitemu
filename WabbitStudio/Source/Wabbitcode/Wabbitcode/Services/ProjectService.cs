using System;
using System.Collections.Generic;
using System.IO;
using Revsoft.Wabbitcode.Interface;
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
					CurrentProjectChanged(value, EventArgs.Empty);
			}
		}

		public void InitService(params Object[] objects)
		{
			OpenProjects = new List<IProject>();
			CurrentProject = new Project.Project(true);
			if (!File.Exists(pathsService.TemplatesConfig))
			{
				//TODO: check we don't need to write this to disk
				string templateXML = ResourceService.GetResource("Templates.xml");
				using (var sw = new StreamWriter(pathsService.TemplatesConfig))
				{
					sw.Write(templateXML);
					sw.Flush();
				}
			}
		}

		public void DestroyService() { }

		public delegate void ChangingHandler(object sender, EventArgs e);
		public event ChangingHandler CurrentProjectChanged;

		public delegate void OpenedHandler(object sender, EventArgs e);
		public event OpenedHandler ProjectOpened;

		public void OpenProject(string fileName)
		{
			IProject project;
			using (FileStream stream = new FileStream(fileName, FileMode.Open))
			{
				project = new Project.Project(stream, fileName);
			}
			CurrentProject = project;
			OpenProjects.Add(project);
			ProjectOpened(project, EventArgs.Empty);
		}
	}
}
