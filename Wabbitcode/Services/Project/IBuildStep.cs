namespace Revsoft.Wabbitcode.Services.Project
{
	using System;

	public interface IBuildStep : ICloneable, IComparable
	{
		string Description
		{
			get;
		}

		string InputFile
		{
			get;
			set;
		}

		string OutputText
		{
			get;
		}

		int StepNumber
		{
			get;
			set;
		}

		/// <summary>
		/// Runs the build step
		/// </summary>
		/// <returns>Returns true if success, false otherwise</returns>
		bool Build(IProject project);
	}
}