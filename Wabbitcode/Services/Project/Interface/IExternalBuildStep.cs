using System;
using Revsoft.Wabbitcode.Services.Project.Interface;

namespace Revsoft.Wabbitcode.Services.Project
{
    interface IExternalBuildStep : IBuildStep
    {
        string Arguments { get; }
        Action<string> Callback { get; set; }
    }
}
