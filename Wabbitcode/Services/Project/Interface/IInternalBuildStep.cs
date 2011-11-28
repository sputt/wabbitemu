using System;
using Revsoft.Wabbitcode.Interface;

namespace Revsoft.Wabbitcode.Services.Project.Interface
{
    public interface IInternalBuildStep : IBuildStep
    {
        string InputFileRelative { get; }
        Revsoft.Wabbitcode.Utilities.FilePath OutputFile { get; set; }
        string OutputFileRelative { get; }
    }
}
