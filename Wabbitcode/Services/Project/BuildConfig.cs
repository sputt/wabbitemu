using System;
using System.Collections.ObjectModel;

namespace Revsoft.Wabbitcode.Services.Project
{
    public class BuildConfig : ICloneable
    {
        private readonly Collection<IBuildStep> _steps = new Collection<IBuildStep>();

        public BuildConfig(string name)
        {
            Name = name;
        }

        public string Name { get; private set; }

        public string OutputText { get; private set; }

        public ReadOnlyCollection<IBuildStep> Steps
        {
            get { return new ReadOnlyCollection<IBuildStep>(_steps); }
        }

        public bool Build(IProject project)
        {
            bool succeeded = true;
            OutputText = string.Empty;

            foreach (IBuildStep step in _steps)
            {
                succeeded &= step.Build(project);
                OutputText += step.OutputText;
            }

            return succeeded;
        }

        public object Clone()
        {
            BuildConfig clone = new BuildConfig(Name);

            foreach (IBuildStep step in _steps)
            {
                clone._steps.Add(step);
            }

            return clone;
        }

        private bool Equals(BuildConfig other)
        {
            return string.Equals(Name, other.Name) && Equals(_steps, other._steps);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                return ((Name != null ? Name.GetHashCode() : 0) * 397) ^ (_steps != null ? _steps.GetHashCode() : 0);
            }
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj))
            {
                return false;
            }

            if (ReferenceEquals(this, obj))
            {
                return true;
            }

            return obj.GetType() == GetType() && Equals((BuildConfig) obj);
        }

        public void AddStep(IBuildStep step)
        {
            if (_steps.Contains(step))
            {
                throw new ArgumentException("Step is already added");
            }
            _steps.Add(step);
        }

        public void RemoveStep(IBuildStep step)
        {
            _steps.Remove(step);
        }

        public override string ToString()
        {
            return Name;
        }
    }
}