using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Classes;
using System.Xml;
using System.IO;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class BuildSystem
	{
		private int currentConfigIndex = 0;
		public int CurrentConfigIndex
		{
			get { return currentConfigIndex; }
			set { currentConfigIndex = value; }
		}

		public BuildConfig CurrentConfig
		{
			get
			{
				if (currentConfigIndex > buildConfigs.Count - 1 || currentConfigIndex == -1)
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
				if (CurrentConfig == null)
					return null;
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
		
		public void Build(bool silent = false)
		{
            if (buildConfigs.Count < 1 || currentConfigIndex == -1)
            {
                System.Windows.Forms.MessageBox.Show("No config set up");
            }
            else
            {
                buildConfigs[currentConfigIndex].Build(silent);
            }
		}

		internal void CreateXML(XmlTextWriter writer)
		{
			writer.WriteStartElement("BuildSystem");
			string includes = "";
			string projFile = ProjectService.ProjectFile;
            foreach (string include in ProjectService.IncludeDirs)
            {
                if (!string.IsNullOrEmpty(include))
                {
                    if (!Path.IsPathRooted(include))
                    {
                        includes += include + ";";
                    }
                    else
                    {
                        includes += FileOperations.GetRelativePath(projFile, include) + ";";
                    }
                }
            }
			writer.WriteAttributeString("IncludeDirs", includes);
            foreach (BuildConfig config in buildConfigs)
            {
                writer.WriteStartElement(config.Name);
                foreach (IBuildStep step in config.Steps)
                {
                    writer.WriteStartElement(step.GetType().Name);
                    writer.WriteAttributeString("StepNum", step.StepNumber.ToString());
                    writer.WriteAttributeString("InputFile", FileOperations.GetRelativePath(projFile, step.InputFile));
                    if (step is ExternalBuildStep)
                    {
                        writer.WriteAttributeString("Arguments", ((ExternalBuildStep)step).Arguments);
                    }
                    else if (step is InternalBuildStep)
                    {
                        var intStep = (InternalBuildStep)step;
                        writer.WriteAttributeString("OutputFile", FileOperations.GetRelativePath(projFile, intStep.OutputFile));
                        writer.WriteAttributeString("StepType", Convert.ToInt16(intStep.StepType).ToString());
                    }
                    writer.WriteEndElement();
                }
                writer.WriteEndElement();
            }
			writer.WriteEndElement();
		}

		internal void ReadXML(XmlTextReader reader)
		{
			string root = ProjectService.ProjectDirectory;
			reader.MoveToNextElement();
            if (reader.Name != "BuildSystem")
            {
                throw new ArgumentException("Invalid XML Format");
            }
			string[] includeDirs = reader.GetAttribute("IncludeDirs").Split(';');
            foreach (string include in includeDirs)
            {
                if (!string.IsNullOrEmpty(include))
                {
                    ProjectService.IncludeDirs.Add((Uri.UnescapeDataString(new Uri(Path.Combine(root, include)).AbsolutePath)));
                }
            }
            BuildConfig configToAdd = null;
			while (reader.MoveToNextElement())
			{
                if (reader.Name.Contains("Step"))
                {
                    if (configToAdd == null)
                    {
                        throw new ArgumentException("Invalid XML Format");
                    }
                    int count = Convert.ToInt32(reader.GetAttribute("StepNum"));
                    string inputFile = reader.GetAttribute("InputFile");
                    switch (reader.Name)
                    {
                        case "ExternalBuildStep":
                            string arguments = reader.GetAttribute("Arguments");
                            ExternalBuildStep exstep = new ExternalBuildStep(count,
                                            Path.Combine(root, inputFile), arguments);
                            configToAdd.Steps.Add(exstep);
                            break;
                        case "InternalBuildStep":
                            string outputFile = reader.GetAttribute("OutputFile");
                            StepType type = (StepType)Convert.ToInt16(reader.GetAttribute("StepType"));
                            InternalBuildStep instep = new InternalBuildStep(count, type,
                                                        Path.Combine(root, inputFile),
                                                        Path.Combine(root, outputFile));
                            configToAdd.Steps.Add(instep);
                            break;
                        default:
                            throw new ArgumentException("Invalid XML Format");
                    }
                }
                else
                {
                    string configName = reader.Name;
                    configToAdd = new BuildConfig(configName);
                    buildConfigs.Add(configToAdd);
                    if (reader.IsEmptyElement)
                    {
                        continue;
                    }
                }
			}
		}
	}

}
