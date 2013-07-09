namespace Revsoft.Wabbitcode.Services.Project
{
    using Revsoft.Wabbitcode.Classes;
    using Revsoft.Wabbitcode.Exceptions;
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Xml;

    public class BuildSystem : IBuildSystem
    {
        private List<BuildConfig> buildConfigs = new List<BuildConfig>();
        private int currentConfigIndex = 0;
        private string outputText = String.Empty;

        public BuildSystem(bool CreateDefaults)
        {
            if (CreateDefaults)
            {
                BuildConfig debug = new BuildConfig("Debug");
                BuildConfig release = new BuildConfig("Release");
                this.buildConfigs.Add(debug);
                this.buildConfigs.Add(release);
            }
        }

        public List<BuildConfig> BuildConfigs
        {
            get
            {
                return this.buildConfigs;
            }
        }

        public BuildConfig CurrentConfig
        {
            get
            {
                if (this.currentConfigIndex > this.buildConfigs.Count - 1 || this.currentConfigIndex == -1)
                {
                    return null;
                }
                return this.buildConfigs[this.currentConfigIndex];
            }

            set
            {
                this.currentConfigIndex = this.buildConfigs.IndexOf(value);
            }
        }

        public int CurrentConfigIndex
        {
            get
            {
                return this.currentConfigIndex;
            }
            set
            {
                this.currentConfigIndex = value;
            }
        }

        public string MainFile
        {
            get
            {
                if (this.CurrentConfig == null)
                {
                    return null;
                }

                int counter = this.CurrentConfig.Steps.Count - 1;
                if (counter < 0)
                {
                    return null;
                }

                InternalBuildStep step = null;
                while (counter >= 0)
                {
                    if (this.CurrentConfig.Steps[counter].GetType() == typeof(InternalBuildStep))
                    {
                        step = (InternalBuildStep)this.CurrentConfig.Steps[counter];
                        break;
                    }

                    counter--;
                }

                if (counter < 0)
                {
                    return null;
                }

                return step.InputFile;
            }
        }

        public string MainOutput
        {
            get
            {
                int counter = this.CurrentConfig.Steps.Count - 1;
                if (counter < 0)
                {
                    return null;
                }
                InternalBuildStep step = null;
                while (counter >= 0)
                {
                    if (this.CurrentConfig.Steps[counter].GetType() == typeof(InternalBuildStep))
                    {
                        step = (InternalBuildStep)this.CurrentConfig.Steps[counter];
                        break;
                    }

                    counter--;
                }

                if (counter < 0)
                {
                    return null;
                }
                return step.OutputFile;
            }
        }

        public string OutputText
        {
            get
            {
                return this.outputText;
            }
        }

        /// <summary>
        /// Builds the build system according to the BuildConfig
        /// </summary>
        /// <param name="silent">Whether or not to </param>
        /// <returns></returns>
        public bool Build()
        {
            if (this.buildConfigs.Count < 1 || this.currentConfigIndex == -1)
            {
                throw new MissingConfigException("Missing config");
            }
            else
            {
                BuildConfig config = this.buildConfigs[this.currentConfigIndex];
                bool succeeded = config.Build();
                this.outputText = config.OutputText;
                return succeeded;
            }
        }

        public void ReadXML(XmlTextReader reader)
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
                    ProjectService.IncludeDirs.Add(Uri.UnescapeDataString(new Uri(Path.Combine(root, include)).AbsolutePath));
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
                        ExternalBuildStep exstep = new ExternalBuildStep(
                            count,
                            Path.Combine(root,
                                         inputFile),
                            arguments);
                        configToAdd.Steps.Add(exstep);
                        break;
                    case "InternalBuildStep":
                        string outputFile = reader.GetAttribute("OutputFile");
                        BuildStepType type = (BuildStepType)Convert.ToInt16(reader.GetAttribute("StepType"));
                        InternalBuildStep instep = new InternalBuildStep(
                            count,
                            type,
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
                    this.buildConfigs.Add(configToAdd);
                    if (reader.IsEmptyElement)
                    {
                        continue;
                    }
                }
            }
        }

        public void WriteXML(XmlTextWriter writer)
        {
            writer.WriteStartElement("BuildSystem");
            string includes = String.Empty;
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
            foreach (BuildConfig config in this.buildConfigs)
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
    }
}