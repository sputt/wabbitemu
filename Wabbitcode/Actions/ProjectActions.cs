using System.Windows.Forms;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Actions
{
    public class CreateNewProjectAction : AbstractUiAction
    {
        public override void Execute()
        {
            NewProjectDialog template = new NewProjectDialog();
            template.ShowDialog();
        }
    }

    public class SaveProjectAction : AbstractUiAction
    {
        private readonly IProjectService _projectService;

        public SaveProjectAction()
        {
            _projectService = DependencyFactory.Resolve<IProjectService>();
        }

        public override void Execute()
        {
            _projectService.SaveProject();
        }
    }

    public class CloseProjectAction : AbstractUiAction
    {
        private readonly IProjectService _projectService;

        public CloseProjectAction()
        {
            _projectService = DependencyFactory.Resolve<IProjectService>();
        }

        public override void Execute()
        {
            DialogResult result = DialogResult.No;
            if (_projectService.Project.NeedsSave && !Settings.Default.AutoSaveProject)
            {
                result = MessageBox.Show("Would you like to save your changes to the project file?", "Save project?", MessageBoxButtons.YesNo, MessageBoxIcon.None);
            }

            if (result == DialogResult.Yes || Settings.Default.AutoSaveProject)
            {
                _projectService.SaveProject();
            }

            _projectService.CloseProject();
        }
    }

    public class AddNewFileAction : AbstractUiAction
    {
        private readonly ProjectViewer _projectViewer;

        public AddNewFileAction(ProjectViewer projectViewer)
        {
            _projectViewer = projectViewer;
        }

        public override void Execute()
        {
            RenameForm newNameForm = new RenameForm
            {
                Text = "New File"
            };
            var result = newNameForm.ShowDialog() != DialogResult.OK;
            newNameForm.Dispose();
            if (result)
            {
                return;
            }

            string name = newNameForm.NewText;
            _projectViewer.AddNewFile(name);
        }
    }

    public class AddExistingFileAction : AbstractUiAction
    {
        private readonly ProjectViewer _projectViewer;

        public AddExistingFileAction(ProjectViewer projectViewer)
        {
            _projectViewer = projectViewer;
        }

        public override void Execute()
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                CheckFileExists = true,
                DefaultExt = "*.asm",
                Filter = "All Know File Types | *.asm; *.z80; *.inc; |Assembly Files (*.asm)|*.asm|*.z80" +
                         " Assembly Files (*.z80)|*.z80|Include Files (*.inc)|*.inc|All Files(*.*)|*.*",
                FilterIndex = 0,
                Multiselect = true,
                RestoreDirectory = true,
                Title = "Add Existing File",
            };
            DialogResult result = openFileDialog.ShowDialog();
            if (result != DialogResult.OK)
            {
                return;
            }

            foreach (string file in openFileDialog.FileNames)
            {
                _projectViewer.AddExistingFile(new FilePath(file));
            }
        }
    }
}
