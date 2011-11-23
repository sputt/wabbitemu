using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Interface.Services
{
	public interface IProjectService : IService
	{
		IProject CurrentProject { get; set; }
		event ProjectService.ChangingHandler CurrentProjectChanged;
		void OpenProject(string fileName);
		List<IProject> OpenProjects { get; }
		event ProjectService.OpenedHandler ProjectOpened;
	}
}
