using System.Globalization;
using System.Threading.Tasks;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Actions
{
    public class BuildAction : AbstractUiAction
    {
        private readonly IAssemblerService _assemblerService;
        private readonly IDebuggerService _debuggerService;
        private readonly IProjectService _projectService;
        private readonly IStatusBarService _statusBarService;

        public BuildAction()
        {
            _assemblerService = ServiceFactory.Instance.GetServiceInstance<IAssemblerService>();
            _debuggerService = ServiceFactory.Instance.GetServiceInstance<IDebuggerService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
            _statusBarService = ServiceFactory.Instance.GetServiceInstance<IStatusBarService>();
        }
        public override void Execute()
        {
            if (_debuggerService.CurrentDebugger != null)
            {
                if (MessageBox.Show("Do you want to stop debugging?", "Debugging", MessageBoxButtons.YesNo) == DialogResult.No)
                {
                    return;
                }

                _debuggerService.EndDebugging();
            }

            _statusBarService.SetText(string.Format(CultureInfo.CurrentCulture, "Building {0}", _projectService.Project.ProjectName));
            _assemblerService.AssemblerProjectFinished += AssemblerService_AssemblerProjectFinished;
            Task.Factory.StartNew(() => _assemblerService.AssembleProject(_projectService.Project));
        }

        private void AssemblerService_AssemblerProjectFinished(object sender, AssemblyFinishProjectEventArgs e)
        {
            _assemblerService.AssemblerProjectFinished -= AssemblerService_AssemblerProjectFinished;
            string statusText = e.Output.Succeeded ? "Build Succeeded" : "Build Failed";
            _statusBarService.SetText(statusText);
        }
    }
}
