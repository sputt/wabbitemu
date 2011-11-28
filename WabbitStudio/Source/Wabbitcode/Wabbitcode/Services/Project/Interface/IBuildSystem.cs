using System;
using System.Collections.Generic;
using System.Xml;

namespace Revsoft.Wabbitcode.Services.Project.Interface
{
	public interface IBuildSystem
	{
		IList<IBuildConfig> BuildConfigs { get; }
		IBuildConfig CurrentConfig { get; set; }
		string MainFile { get; }
		string MainOutput { get; }
		IProject Project { get; }

		void Build();
		void CreateXML(XmlTextWriter writer);
		void ReadXML(XmlTextReader reader);
	}
}
