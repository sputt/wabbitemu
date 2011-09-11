using System;
using System.Collections.Generic;
using System.Xml;
using Revsoft.Wabbitcode.Interface;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class BuildSystem : IBuildSystem
	{
		private int currentConfigIndex = 0;

		public IBuildConfig CurrentConfig
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

		public string MainFile
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
				return step.InputFile;
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

		public BuildSystem(IProject project, bool CreateDefaults)
		{
			if (CreateDefaults)
			{
				BuildConfig debug = new BuildConfig(project, "Debug");
				BuildConfig release = new BuildConfig(project, "Release");
				buildConfigs.Add(debug);
				buildConfigs.Add(release);
			}
			Project = project;
		}

		IList<IBuildConfig> buildConfigs = new List<IBuildConfig>();
		public IList<IBuildConfig> BuildConfigs
		{
			get { return buildConfigs; }
		}

		public IProject Project { get; private set; }
		
		public void Build()
		{
			if (buildConfigs.Count < 1 || currentConfigIndex == -1)
				throw new Exception("No config set up");
			else
				buildConfigs[currentConfigIndex].Build();
		}

		public void CreateXML(XmlTextWriter writer)
		{
			int counter = 0;
			writer.WriteStartElement("BuildSystem");
			string includes = "";
			foreach(string include in Project.IncludeDirs)
				if (!string.IsNullOrEmpty(include))
				includes += include + ";";
			writer.WriteAttributeString("IncludeDirs", includes);
			foreach (IBuildConfig config in buildConfigs)
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
						ExternalBuildStep eStep = (ExternalBuildStep) step;
						writer.WriteAttributeString("Arguments", eStep.Arguments);
					}
					else if (type == typeof(InternalBuildStep))
					{
						InternalBuildStep iStep = (InternalBuildStep) step;
						writer.WriteAttributeString("OutputFile", iStep.OutputFile);
						writer.WriteAttributeString("StepType", Convert.ToInt16(iStep.StepType).ToString());
					}
					writer.WriteEndElement();
				}
				writer.WriteEndElement();
			}
			writer.WriteEndElement();
		}

		public void ReadXML(XmlTextReader reader)
		{
			reader.MoveToNextElement();
			if (reader.Name != "BuildSystem")
				throw new ArgumentException("Invalid XML Format");
			string[] includeDirs = reader.GetAttribute("IncludeDirs").Split(';');
			foreach (string include in includeDirs)
				if (!string.IsNullOrEmpty(include))
					this.Project.IncludeDirs.Add(include);
			while (reader.MoveToNextElement())
			{
				string configName = reader.Name;
				BuildConfig configToAdd = new BuildConfig(Project, configName);
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
						InternalBuildStep instep = new InternalBuildStep(Project, count, type, inputFile, outputFile);
						configToAdd.Steps.Add(instep);
						break;
					default:
						throw new ArgumentException("Invalid XML Format");
				}
			}
		}
	}

}
