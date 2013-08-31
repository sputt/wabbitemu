using System.Collections.Generic;
using System.Xml;
using Revsoft.Wabbitcode.Services.Interface;

namespace Revsoft.Wabbitcode.Services.Project
{
	public interface IBuildSystem
	{
		IAssemblerService AssemblerService
		{
			get;
			set;
		}

		IList<BuildConfig> BuildConfigs
		{
			get;
		}

		BuildConfig CurrentConfig
		{
			get;
			set;
		}

		int CurrentConfigIndex
		{
			get;
			set;
		}

		string MainFile
		{
			get;
		}

		string OutputText
		{
			get;
		}

		string ProjectOutput
		{
			get;
			set;
		}

		string LabelOutput
		{
			get;
			set;
		}

		string ListOutput
		{
			get;
			set;
		}

		void CreateDefaultConfigs();

		bool Build();

		void ReadXML(XmlTextReader reader);

		void WriteXML(XmlTextWriter writer);
	}
}