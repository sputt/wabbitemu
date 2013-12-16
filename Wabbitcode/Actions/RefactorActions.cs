using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.Interfaces;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Actions
{
    public class RefactorRenameAction : AbstractUiAction
    {
        private readonly IDockingService _dockingService;
        private readonly IProjectService _projectService;

        public RefactorRenameAction()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
        }

        public override void Execute()
        {
            ITextEditor editor = _dockingService.ActiveDocument as ITextEditor;
            if (editor == null)
            {
                return;
            }

            RefactorForm form = new RefactorForm(editor, _projectService);
            form.ShowDialog();
        }
    }
}
