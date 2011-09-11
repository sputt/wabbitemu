using System;
using Revsoft.Wabbitcode.Interface;

namespace Revsoft.Wabbitcode.Services
{
	public interface IPathsService : IService
	{
		string DockConfig { get; }
		string IncludeDirectory { get; }
		string ProjectDirectory { get; }
		string TemplatesConfig { get; }
		string TemplatesDirectory { get; }
		string WabbitcodeDirectory { get; }
	}
}
