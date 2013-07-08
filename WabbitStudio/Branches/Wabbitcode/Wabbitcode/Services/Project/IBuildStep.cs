namespace Revsoft.Wabbitcode.Services.Project
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.IO;
    using System.Linq;
    using System.Text;

    public interface IBuildStep : ICloneable
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
        bool Build();
    }
}