using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Project.Interface;
using System.IO;

namespace Revsoft.Wabbitcode.Interface.Services
{
	public interface IProjectService : IService
	{
		IProject CurrentProject { get; set; }
		event ProjectService.ChangingHandler CurrentProjectChanged;
		void OpenProject(Stream stream, string fileName);
		List<IProject> OpenProjects { get; }
		event ProjectService.OpenedHandler ProjectOpened;
	}
}
