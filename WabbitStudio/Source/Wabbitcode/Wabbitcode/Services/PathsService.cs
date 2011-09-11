using System;
using System.IO;
using System.Windows;
using Revsoft.Wabbitcode.Interface;

namespace Revsoft.Wabbitcode.Services
{
	public class PathsService : IPathsService
	{
		const string dockConfigPath = "DockConfig.xml";
		const string templatesPath = "TemplatesConfig.xml";
		const string wabbitcodeDirName = "Wabbitcode";
		const string templateDirName = "Templates";
		const string includeDirName = "Includes";
		const string projectsName = "Projects";
		private string ApplicationAppData
		{
			get
			{
				return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), Application.ResourceAssembly.GetName().Name);
			}
		}

		public string TemplatesConfig
		{
			get
			{
				return Path.Combine(TemplatesDirectory, templatesPath);
			}
		}

		public string DockConfig
		{
			get
			{
				return Path.Combine(ApplicationAppData, dockConfigPath);
			}
		}

		public string WabbitcodeDirectory
		{
			get
			{
				return Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments), wabbitcodeDirName);
			}
		}

		public string ProjectDirectory
		{
			get
			{
				return Path.Combine(WabbitcodeDirectory, projectsName);
			}
		}

		public string TemplatesDirectory
		{
			get
			{
				return Path.Combine(WabbitcodeDirectory, templateDirName);
			}
		}

		public string IncludeDirectory
		{
			get
			{
				return Path.Combine(WabbitcodeDirectory, includeDirName);
			}
		}

		public void InitService(params Object[] objects)
		{
			Directory.CreateDirectory(ApplicationAppData);
			Directory.CreateDirectory(WabbitcodeDirectory);
			Directory.CreateDirectory(TemplatesDirectory);
			Directory.CreateDirectory(IncludeDirectory);
		}

		public void DestroyService() { }
	}
}
