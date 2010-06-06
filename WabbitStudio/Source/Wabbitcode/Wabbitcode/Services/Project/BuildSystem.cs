using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Classes;
using System.Xml;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class BuildSystem
	{
		private int currentConfigIndex = 0;
		public int CurrentConfigIndex
		{
			get { return currentConfigIndex; }
		}

		public BuildConfig CurrentConfig
		{
			get
			{
				if (currentConfigIndex == -1)
					return null;
				return buildConfigs[currentConfigIndex];
			}
			set
			{
				currentConfigIndex = buildConfigs.IndexOf(value);
			}
		}

		public string MainOutput
		{
			get
			{
				int counter = CurrentConfig.Steps.Count - 1;
				if (counter < 0)
					return null;
				InternalBuildStep step = null;
				while (counter >= 0)
				{
					if (CurrentConfig.Steps[counter].GetType() == typeof(InternalBuildStep))
					{
						step = (InternalBuildStep)CurrentConfig.Steps[counter];
						break;
					}
					counter--;
				}
				if (counter < 0)
					return null;
				return step.OutputFile;
			}
		}

		public BuildSystem(bool CreateDefaults)
		{
			if (CreateDefaults)
			{
				BuildConfig debug = new BuildConfig("Debug");
				BuildConfig release = new BuildConfig("Release");
				buildConfigs.Add(debug);
				buildConfigs.Add(release);
			}
		}

		List<BuildConfig> buildConfigs = new List<BuildConfig>();
		public List<BuildConfig> BuildConfigs
		{
			get { return buildConfigs; }
		}
		
		public void Build()
		{
			if (buildConfigs.Count < 1 || currentConfigIndex == -1)
				System.Windows.Forms.MessageBox.Show("No config set up");
			else
				buildConfigs[currentConfigIndex].Build();
		}

		internal void CreateXML(XmlTextWriter writer)
		{
			int counter = 0;
			writer.WriteStartElement("BuildSystem");
			foreach (BuildConfig config in buildConfigs)
			{
				writer.WriteStartElement(config.Name);
				foreach (IBuildStep step in config.Steps)
				{
					Type type = step.GetType();
					writer.WriteStartElement(type.Name);
					writer.WriteAttributeString("Count", counter.ToString());
					writer.WriteAttributeString("InputFile", step.InputFile);
					if (type == typeof(ExternalBuildStep))
					{
						writer.WriteAttributeString("Arguments", ((ExternalBuildStep)step).Arguments);
					}
					else if (type == typeof(InternalBuildStep))
					{
						writer.WriteAttributeString("OutputFile", ((InternalBuildStep)step).OutputFile);
						writer.WriteAttributeString("StepType", Convert.ToInt16(((InternalBuildStep)step).StepType).ToString());
					}
					writer.WriteEndElement();
				}
				writer.WriteEndElement();
			}
			writer.WriteEndElement();
		}

		internal void ReadXML(XmlTextReader reader)
		{
			reader.MoveToNextElement();
			if (reader.Name != "BuildSystem")
				throw new ArgumentException("Invalid XML Format");
			while (reader.MoveToNextElement())
			{
				string configName = reader.Name;
				BuildConfig configToAdd = new BuildConfig(configName);
				buildConfigs.Add(configToAdd);
				if (reader.IsEmptyElement)
					continue;
				reader.MoveToNextElement();
				int count = Convert.ToInt32(reader.GetAttribute("Count"));
				string inputFile = reader.GetAttribute("InputFile");
				switch (reader.Name)
				{
					case "ExternalBuildStep":
						string arguments = reader.GetAttribute("Arguments");
						ExternalBuildStep exstep = new ExternalBuildStep(count, inputFile, arguments);
						configToAdd.Steps.Add(exstep);
						break;
					case "InternalBuildStep":
						string outputFile = reader.GetAttribute("OutputFile");
						StepType type = (StepType)Convert.ToInt16(reader.GetAttribute("StepType"));
						InternalBuildStep instep = new InternalBuildStep(count, type, inputFile, outputFile);
						configToAdd.Steps.Add(instep);
						break;
					default:
						throw new ArgumentException("Invalid XML Format");
				}
			}
		}
	}

}
