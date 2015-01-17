using System.Linq;
using System.Windows.Forms;
using Microsoft.Win32;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.TextEditor.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Actions
{
    public class ToggleBreakpointAction : AbstractUiAction
    {
        private readonly IDockingService _dockingService;

        public ToggleBreakpointAction()
        {
            _dockingService = DependencyFactory.Resolve<IDockingService>();
        }

        protected override void Execute()
        {
            ITextEditor activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            FilePath fileName = activeTextEditor.FileName;
            int lineNum = activeTextEditor.CaretLine;
            WabbitcodeBreakpoint breakpoint = WabbitcodeBreakpointManager.Breakpoints
                .SingleOrDefault(b => b.File == fileName && b.LineNumber == lineNum);
            if (breakpoint == null)
            {
                WabbitcodeBreakpointManager.AddBreakpoint(fileName, lineNum);
            }
            else
            {
                WabbitcodeBreakpointManager.RemoveBreakpoint(fileName, lineNum);
            }
        }
    }

    public class StartDebuggerAction : AbstractUiAction
    {
        private readonly IDebuggerService _debuggerService;
        private readonly BuildAction _buildAction;

        public StartDebuggerAction()
        {
            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
            _buildAction = new BuildAction();
        }

        protected override void Execute()
        {
            if (_debuggerService.CurrentDebugger == null)
            {
                _buildAction.BuildFinished += BuildAction_BuildFinished;
                RunCommand(_buildAction);
            }
            else
            {
                _debuggerService.CurrentDebugger.Run();
            }
        }

        private void BuildAction_BuildFinished(object sender, AssemblyFinishEventArgs e)
        {
            if (!e.Output.Succeeded)
            {
                if (DockingService.ShowMessageBox(null, "There were errors assembling. Would you like to continue and try to debug?",
                    "Continue", MessageBoxButtons.YesNo, MessageBoxIcon.Error) == DialogResult.No)
                {
                    return;
                }
            }

            try
            {
                _debuggerService.StartDebugging();
            }
            catch (MissingRomException)
            {
                bool hasRom = PromptForRomFile();
                if (hasRom)
                {
                    Execute();
                }
            }
            catch (DebuggingException ex)
            {
                DockingService.ShowError("Unable to start debugging", ex);
            }
        }

        private static bool PromptForRomFile()
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                CheckFileExists = true,
                DefaultExt = "*.rom",
                Filter = "All Know File Types | *.rom; *.sav; |ROM Files (*.rom)|*.rom|" +
                         "Savestate Files (*.sav)|*.sav|All Files(*.*)|*.*",
                FilterIndex = 0,
                Multiselect = true,
                RestoreDirectory = true,
                Title = "Select new ROM file",
            };

            if (openFileDialog.ShowDialog() != DialogResult.OK)
            {
                return false;
            }

            SaveRomPathRegistry(openFileDialog.FileName);
            return true;
        }

        private static void SaveRomPathRegistry(string romFileName)
        {
            RegistryKey romKey = null;
            try
            {
                romKey = Registry.CurrentUser.OpenSubKey("Software\\Wabbitemu", true);
                if (romKey != null)
                {
                    romKey.SetValue("rom_path", romFileName);
                }
            }
            finally
            {
                if (romKey != null)
                {
                    romKey.Close();
                }
            }
        }
    }

    public class PauseDebuggerAction : AbstractUiAction
    {
        private readonly IDebuggerService _debuggerService;

        public PauseDebuggerAction()
        {
            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
        }

        protected override void Execute()
        {
            if (_debuggerService.CurrentDebugger == null)
            {
                return;
            }

            try
            {
                _debuggerService.CurrentDebugger.Pause();
            }
            catch (DebuggingException)
            {
                MessageBox.Show("Unable to pause at this point.");
            }
        }
    }

    public class StopDebuggerAction : AbstractUiAction
    {
        private readonly IDebuggerService _debuggerService;

        public StopDebuggerAction()
        {
            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
        }

        protected override void Execute()
        {
            if (_debuggerService.CurrentDebugger == null)
            {
                return;
            }
            _debuggerService.EndDebugging();
        }
    }

    public class RestartDebuggerAction : AbstractUiAction
    {
        private readonly IDebuggerService _debuggerService;

        public RestartDebuggerAction()
        {
            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
        }

        protected override void Execute()
        {
            if (_debuggerService.CurrentDebugger == null)
            {
                return;
            }

            _debuggerService.EndDebugging();
            _debuggerService.StartDebugging();
        }
    }

    public class StepDebuggerAction : AbstractUiAction
    {
        private readonly IDebuggerService _debuggerService;

        public StepDebuggerAction()
        {
            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
        }

        protected override void Execute()
        {
            if (_debuggerService.CurrentDebugger == null)
            {
                return;
            }

            _debuggerService.CurrentDebugger.Step();
        }
    }

    public class StepOverDebuggerAction : AbstractUiAction
    {
        private readonly IDebuggerService _debuggerService;

        public StepOverDebuggerAction()
        {
            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
        }

        protected override void Execute()
        {
            if (_debuggerService.CurrentDebugger == null)
            {
                return;
            }

            _debuggerService.CurrentDebugger.StepOver();
        }
    }

    public class StepOutDebuggerAction : AbstractUiAction
    {
        private readonly IDebuggerService _debuggerService;

        public StepOutDebuggerAction()
        {
            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
        }

        protected override void Execute()
        {
            if (_debuggerService.CurrentDebugger == null)
            {
                return;
            }

            _debuggerService.CurrentDebugger.StepOut();
        }
    }

    public class SetNextStatementAction : AbstractUiAction
    {
        private readonly IDebuggerService _debuggerService;
        private readonly IDockingService _dockingService;

        public SetNextStatementAction()
        {
            _dockingService = DependencyFactory.Resolve<IDockingService>();
            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
        }

        protected override void Execute()
        {
            ITextEditor editor = _dockingService.ActiveDocument as ITextEditor;
            if (editor != null)
            {
                _debuggerService.CurrentDebugger.SetPCToSelect(editor.FileName, editor.CaretLine + 1);
            }
        }
    }
}