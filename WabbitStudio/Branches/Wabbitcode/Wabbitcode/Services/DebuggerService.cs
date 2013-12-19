using System;
using System.IO;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Services.Utils;

namespace Revsoft.Wabbitcode.Services
{
    [ServiceDependency(typeof(IProjectService))]
    internal class DebuggerService : IDebuggerService
    {
        private readonly IProjectService _projectService;

        public event EventHandler<DebuggingEventArgs> OnDebuggingStarted;
        public event EventHandler<DebuggingEventArgs> OnDebuggingEnded;

        public IWabbitcodeDebugger CurrentDebugger { get; private set; }

        public void StartDebugging()
        {
            IProject project = _projectService.Project;
            if (project.IsInternal)
            {
                throw new DebuggingException("Debugging single files is not supported");
            }

            string outputFile = GetOutputFileDetails(project);
            CurrentDebugger = new WabbitcodeDebugger(outputFile);


            if (OnDebuggingStarted != null)
            {
                OnDebuggingStarted(this, new DebuggingEventArgs(CurrentDebugger));
            }

            try
            {
                CurrentDebugger.StartDebug();
            }
            catch (DebuggingException)
            {
                CurrentDebugger.Dispose();
                CurrentDebugger = null;
                EndDebugging();
            }
        }

        public void EndDebugging()
        {
            CurrentDebugger.EndDebug();
            CurrentDebugger.Dispose();
            CurrentDebugger = null;

            if (OnDebuggingEnded != null)
            {
                OnDebuggingEnded(this, new DebuggingEventArgs(CurrentDebugger));
            }
        }

        public void GotoCurrentDebugLine()
        {
            throw new NotImplementedException();
        }

        private static string GetOutputFileDetails(IProject project)
        {
            if (string.IsNullOrEmpty(project.BuildSystem.ProjectOutput))
            {
                throw new DebuggingException("No project outputs detected");
            }

            string createdName = project.BuildSystem.ProjectOutput;
            if (!Path.IsPathRooted(createdName))
            {
                createdName = FileOperations.NormalizePath(Path.Combine(project.ProjectDirectory, createdName));
            }

            if (string.IsNullOrEmpty(project.BuildSystem.ListOutput))
            {
                throw new DebuggingException("Missing list file");
            }

            string listName = project.BuildSystem.ListOutput;
            if (string.IsNullOrEmpty(project.BuildSystem.LabelOutput))
            {
                throw new DebuggingException("Missing label file");
            }

            string symName = project.BuildSystem.LabelOutput;

            if (!File.Exists(listName))
            {
                throw new DebuggingException("Error list file does not exist");
            }

            if (!File.Exists(symName))
            {
                throw new DebuggingException("Error label file does not exist");
            }

            return createdName;
        }

        public void DestroyService()
        {
        }

        public void InitService(params object[] objects)
        {
        }

        public DebuggerService(IProjectService projectService)
        {
            _projectService = projectService;
        }
    }
}
