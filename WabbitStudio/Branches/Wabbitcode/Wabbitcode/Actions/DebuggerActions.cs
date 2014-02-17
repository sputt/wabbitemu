using System.Linq;
using System.Windows.Forms;
using Microsoft.Win32;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Interfaces;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Actions
{
    public class ToggleBreakpointAction : AbstractUiAction
    {
        private readonly IDockingService _dockingService;

        public ToggleBreakpointAction()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
        }

        public override void Execute()
        {
            ITextEditor activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            string fileName = activeTextEditor.FileName;
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
        private readonly IAssemblerService _assemblerService;
        private readonly IDebuggerService _debuggerService;

        public StartDebuggerAction()
        {
            _assemblerService = ServiceFactory.Instance.GetServiceInstance<IAssemblerService>();
            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
        }

        public override void Execute()
        {
            if (_debuggerService.CurrentDebugger == null)
            {
                _assemblerService.AssemblerProjectFinished += AssemblerService_AssemblerProjectFinished;
                new BuildAction().Execute();
            }
            else
            {
                _debuggerService.CurrentDebugger.Run();
            }
        }

        private void AssemblerService_AssemblerProjectFinished(object sender, AssemblyFinishProjectEventArgs e)
        {
            _assemblerService.AssemblerProjectFinished -= AssemblerService_AssemblerProjectFinished;
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
            catch (MissingROMException)
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
            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
        }

        public override void Execute()
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
            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
        }

        public override void Execute()
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
            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
        }

        public override void Execute()
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
            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
        }

        public override void Execute()
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
            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
        }

        public override void Execute()
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
            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
        }

        public override void Execute()
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
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
        }

        public override void Execute()
        {
            ITextEditor editor = _dockingService.ActiveDocument as ITextEditor;
            if (editor != null)
            {
                _debuggerService.CurrentDebugger.SetPCToSelect(editor.FileName, editor.CaretLine + 1);
            }
        }
    }
}
