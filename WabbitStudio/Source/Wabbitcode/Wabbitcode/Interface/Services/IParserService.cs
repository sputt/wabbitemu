using System;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Interface.Services
{
	public interface IParserService : IService
	{
		ParserInformation ParseFile(string file);
		ParserInformation ParseFile(string file, string lines, float increment = .05f, Action<double> callback = null);
		IProject Project { get; }
	}
}
