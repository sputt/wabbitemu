using System;
using System.IO;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public class DebuggerService : IDebuggerService
    {
        private readonly IProjectService _projectService;

        public event EventHandler<DebuggingEventArgs> OnDebuggingStarted;
        public event EventHandler<DebuggingEventArgs> OnDebuggingEnded;

        public IWabbitcodeDebugger CurrentDebugger { get; private set; }

        public DebuggerService(IProjectService projectService)
        {
            _projectService = projectService;
        }

        public void StartDebugging()
        {
            IProject project = _projectService.Project;
            if (project.IsInternal)
            {
                throw new DebuggingException("Debugging single files is not supported");
            }

            string outputFile = GetOutputFileDetails(project);
            try
            {
                CurrentDebugger = new WabbitcodeDebugger(outputFile);
            }
            catch (Exception)
            {
                throw new DebuggingException("Unable to create the debuger");
            }


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

        private static string GetOutputFileDetails(IProject project)
        {
            if (string.IsNullOrEmpty(project.BuildSystem.ProjectOutput))
            {
                throw new DebuggingException("No project outputs detected");
            }

            string createdName = project.BuildSystem.ProjectOutput;
            if (!Path.IsPathRooted(createdName))
            {
                createdName = project.ProjectDirectory.Combine(createdName).NormalizePath();
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
    }
}