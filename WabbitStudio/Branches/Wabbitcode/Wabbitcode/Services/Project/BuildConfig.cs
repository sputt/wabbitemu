namespace Revsoft.Wabbitcode.Services.Project
{
    using System;
    using System.Collections.Generic;

    public class BuildConfig : ICloneable
    {
        private string name;
        private string outputText;
        private List<IBuildStep> steps = new List<IBuildStep>();

        public BuildConfig(string name)
        {
            this.name = name;
        }

        public string Name
        {
            get
            {
                return this.name;
            }
        }

        public string OutputText
        {
            get
            {
                return this.outputText;
            }
        }

        public List<IBuildStep> Steps
        {
            get
            {
                return this.steps;
            }
        }

        public bool Build()
        {
            bool succeeded = true;
            this.outputText = String.Empty;
            SortSteps();
            ProjectService.Project.ProjectOutputs.Clear();
            ProjectService.Project.ListOutputs.Clear();
            ProjectService.Project.LabelOutputs.Clear();
            foreach (IBuildStep step in this.steps)
            {
                succeeded &= step.Build();
                this.outputText += step.OutputText;
            }

            return succeeded;
        }

        public object Clone()
        {
            BuildConfig clone = new BuildConfig(this.name);
            clone.steps = new List<IBuildStep>();
            int counter = 0;
            foreach (IBuildStep step in this.steps)
            {
                clone.steps.Add(step);
                counter++;
            }

            return clone;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is BuildConfig))
            {
                return false;
            }

            BuildConfig config = (BuildConfig)obj;
            if (config.name == this.name && config.steps.Count == this.steps.Count)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public void SortSteps()
        {
            this.steps.Sort(SortSteps);
        }

        public override string ToString()
        {
            return this.name;
        }

        private static int SortSteps(IBuildStep step1, IBuildStep step2)
        {
            if (step1 == null)
            {
                if (step2 == null)
                {
                    return 0;
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                if (step2 == null)
                {
                    return 1;
                }
                if (step1.StepNumber == step2.StepNumber)
                {
                    return 0;
                }
                if (step1.StepNumber > step2.StepNumber)
                {
                    return 1;
                }
                else
                {
                    return -1;
                }
            }
        }
    }
}