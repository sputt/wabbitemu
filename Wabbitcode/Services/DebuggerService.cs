using System;
using System.IO;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;
using System.Diagnostics;
using Revsoft.Wabbitcode.TextEditor.Interfaces;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public class DebuggerService : IDebuggerService
    {
        private readonly IProjectService _projectService;
        private readonly IDockingService _dockingService;

        public event EventHandler<DebuggingEventArgs> OnDebuggingStarted;
        public event EventHandler<DebuggingEventArgs> OnDebuggingEnded;

        public IWabbitcodeDebugger CurrentDebugger { get; private set; }

        public DebuggerService(IProjectService projectService, IDockingService dockingService)
        {
            _projectService = projectService;
            _dockingService = dockingService;
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
                Debug.WriteLine("Creating wabbitocde debugger");
                CurrentDebugger = new WabbitcodeDebugger(outputFile);
                CurrentDebugger.DebuggerStep += CurrentDebugger_DebuggerStep;
                CurrentDebugger.DebuggerRunningChanged += CurrentDebugger_DebuggerRunningChanged;
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
                Debug.WriteLine("Starting debug");
                CurrentDebugger.StartDebug();
            }
            catch (DebuggingException)
            {
                EndDebugging();
            }
        }

        public void EndDebugging()
        {
            if (OnDebuggingEnded != null)
            {
                OnDebuggingEnded(this, new DebuggingEventArgs(CurrentDebugger));
            }

            CurrentDebugger.EndDebug();
            CurrentDebugger.Dispose();
            CurrentDebugger = null;
        }

        private void CurrentDebugger_DebuggerRunningChanged(object sender, DebuggerRunningEventArgs e)
        {
            _dockingService.DockPanel.BeginInvoke(() =>
            {
                ITextEditor editor = _dockingService.ActiveDocument as ITextEditor;
                if (editor != null)
                {
                    editor.RemoveDebugHighlight();
                }

                if (e.Running)
                {
                    return;
                }

                AbstractUiAction.RunCommand(new GotoLineAction(e.Location.FileName, e.Location.LineNumber - 1));
                editor = _dockingService.ActiveDocument as ITextEditor;
                if (editor != null)
                {
                    editor.HighlightDebugLine(e.Location.LineNumber - 1);
                }
            });
        }

        private void CurrentDebugger_DebuggerStep(object sender, DebuggerStepEventArgs e)
        {
            _dockingService.DockPanel.BeginInvoke(() =>
            {
                ITextEditor editor = _dockingService.ActiveDocument as ITextEditor;
                if (editor != null)
                {
                    editor.RemoveDebugHighlight();
                }

                AbstractUiAction.RunCommand(new GotoLineAction(e.Location.FileName, e.Location.LineNumber - 1));

                editor = _dockingService.ActiveDocument as ITextEditor;
                if (editor != null)
                {
                    editor.HighlightDebugLine(e.Location.LineNumber - 1);
                }
            });
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