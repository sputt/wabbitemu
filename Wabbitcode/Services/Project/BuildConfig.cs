using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class BuildConfig : ICloneable
	{
		private string name;
		public string Name
		{
			get { return name; }
		}

		List<IBuildStep> steps = new List<IBuildStep>();
		public List<IBuildStep> Steps
		{
			get { return steps; }
		}

		public BuildConfig(string name)
		{
			this.name = name;
		}

		public void SortSteps()
		{
			steps.Sort(SortSteps);
		}

		public void Build()
		{
			SortSteps();
			ProjectService.Project.ProjectOutputs.Clear();
			ProjectService.Project.ListOutputs.Clear();
			ProjectService.Project.LabelOutputs.Clear();
			foreach (IBuildStep step in steps)
				step.Build();
		}

		static int SortSteps(IBuildStep step1, IBuildStep step2)
		{
			if (step1 == null)
			{
				if (step2 == null)
					return 0;
				else
					return -1;
			}
			else
			{
				if (step2 == null)
					return 1;
				if (step1.StepNumber == step2.StepNumber)
					return 0;
				if (step1.StepNumber > step2.StepNumber)
					return 1;
				else
					return -1;
			}
		}

		public override string ToString()
		{
			return name;
		}

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

		public override bool Equals(object obj)
		{
			if (obj.GetType() != typeof(BuildConfig))
				return false;
			BuildConfig config = (BuildConfig)obj;
			if (config.name == this.name && config.steps.Count == this.steps.Count)
				return true;
			else 
				return false;
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
	}
}
