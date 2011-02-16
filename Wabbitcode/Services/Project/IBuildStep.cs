using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.IO;

namespace Revsoft.Wabbitcode.Services.Project
{
	public interface IBuildStep : ICloneable
	{
		int StepNumber { get; set; }
        string InputFile { get; set; }
        string Description { get; }
		bool Build();
	}
}
