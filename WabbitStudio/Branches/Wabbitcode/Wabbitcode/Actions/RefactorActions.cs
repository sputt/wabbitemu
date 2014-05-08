using System.Windows.Forms;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.Interfaces;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Actions
{
    public class RefactorRenameAction : AbstractUiAction
    {
        private readonly IDockingService _dockingService;
        private readonly IFileService _fileService;
        private readonly IProjectService _projectService;

        public RefactorRenameAction()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _fileService = ServiceFactory.Instance.GetServiceInstance<IFileService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
        }

        public override void Execute()
        {
            ITextEditor editor = _dockingService.ActiveDocument as ITextEditor;
            if (editor == null)
            {
                return;
            }

            RefactorRenameForm renameForm = new RefactorRenameForm(editor, _fileService, _projectService);
            bool validRename = renameForm.Initialize();
            if (validRename)
            {
                renameForm.ShowDialog();
            }
            else
            {
                MessageBox.Show(editor as IWin32Window, "Unable to rename symbol at the cursor", "Error", MessageBoxButtons.OK);
            }
        }
    }

    public class RefactorExtractMethodAction : AbstractUiAction
    {
        private readonly IDockingService _dockingService;
        private readonly IProjectService _projectService;

        public RefactorExtractMethodAction()
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

            RefactorExtractMethodForm renameForm = new RefactorExtractMethodForm(editor, _projectService);
            renameForm.ShowDialog();
        }
    }
}
