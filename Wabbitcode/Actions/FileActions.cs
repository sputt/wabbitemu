using System;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Actions
{
    public class CreateNewDocumentAction : AbstractUiAction
    {
        private readonly IDocumentService _documentService;
        private readonly IDockingService _dockingService;

        public CreateNewDocumentAction()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _documentService = ServiceFactory.Instance.GetServiceInstance<IDocumentService>();
        }

        public override void Execute()
        {
            Editor doc = _documentService.CreateNewDocument();
            doc.TabText = "New Document";
            _dockingService.ShowDockPanel(doc);
        }
    }

    public class OpenFileAction : AbstractUiAction
    {
        public override void Execute()
        {
            var openFileDialog = new OpenFileDialog
            {
                CheckFileExists = true,
                DefaultExt = "*.asm",
                Filter = "All Know File Types | *.asm; *.z80; *.wcodeproj| Assembly Files (*.asm)|*.asm|Z80" +
                         " Assembly Files (*.z80)|*.z80 | Include Files (*.inc)|*.inc | Project Files (*.wcodeproj)" +
                         "|*.wcodeproj|All Files(*.*)|*.*",
                FilterIndex = 0,
                RestoreDirectory = true,
                Multiselect = true,
                Title = "Open File",
            };

            if (openFileDialog.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            try
            {
                foreach (var fileName in openFileDialog.FileNames)
                {
                    FileTypeMethodFactory.OpenRegisteredFile(fileName);
                }
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error opening file", ex);
            }
        }
    }

    public class OpenProjectCommand : AbstractUiAction
    {
        private readonly IProjectService _projectService;

        public OpenProjectCommand()
        {
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
        }

        public override void Execute()
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                CheckFileExists = true,
                DefaultExt = "*.wcodeproj",
                Filter = "Project Files (*.wcodeproj)|*.wcodeproj|All Files(*.*)|*.*",
                FilterIndex = 0,
                RestoreDirectory = true,
                Title = "Open Project File",
            };
            try
            {
                if (openFileDialog.ShowDialog() != DialogResult.OK)
                {
                    return;
                }

                string fileName = openFileDialog.FileName;

                if (!_projectService.OpenProject(fileName))
                {
                    _projectService.CreateInternalProject();
                }

                if (Settings.Default.StartupProject == fileName)
                {
                    return;
                }

                if (
                    MessageBox.Show("Would you like to make this your default project?",
                        "Startup Project",
                        MessageBoxButtons.YesNo,
                        MessageBoxIcon.Question) == DialogResult.Yes)
                {
                    Settings.Default.StartupProject = fileName;
                }
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error opening file.", ex);
            }
        }
    }

    public class SaveCommand : AbstractUiAction
    {
        private readonly AbstractFileEditor _editor;

        public SaveCommand()
        {
            _editor = ServiceFactory.Instance.GetServiceInstance<IDockingService>().ActiveDocument as AbstractFileEditor;
        }

        public override void Execute()
        {
            try
            {
                _editor.SaveFile();
            }
            catch (Exception ex)
            {
                DockingService.ShowError("Error saving file.", ex);
            }
        }
    }

    public class SaveAsCommand : AbstractUiAction
    {
        private readonly AbstractFileEditor _editor;

        public SaveAsCommand()
        {
            _editor = ServiceFactory.Instance.GetServiceInstance<IDockingService>().ActiveDocument as AbstractFileEditor;
        }

        public override void Execute()
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog
            {
                DefaultExt = "asm",
                RestoreDirectory = true,
                Filter = "All Know File Types | *.asm; *.z80| Assembly Files (*.asm)|*.asm|Z80" +
                         " Assembly Files (*.z80)|*.z80|All Files(*.*)|*.*",
                FilterIndex = 0,
                Title = "Save File As"
            };

            if (saveFileDialog.ShowDialog() != DialogResult.OK)
            {
                return;
            }
            if (string.IsNullOrEmpty(saveFileDialog.FileName))
            {
                return;
            }

            _editor.SaveFile(saveFileDialog.FileName);
        }
    }

    public class SaveAllCommand : AbstractUiAction
    {
        private readonly IDockingService _dockingService;

        public SaveAllCommand()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
        }

        public override void Execute()
        {
            foreach (AbstractFileEditor child in _dockingService.Documents.OfType<AbstractFileEditor>())
            {
                child.SaveFile();
            }
        }
    }

    public class CloseCommand : AbstractUiAction
    {
        private readonly AbstractFileEditor _editor;

        public CloseCommand()
        {
            _editor = ServiceFactory.Instance.GetServiceInstance<IDockingService>().ActiveDocument as AbstractFileEditor;
        }

        public override void Execute()
        {
            if (_editor == null)
            {
                return;
            }

            _editor.Close();
        }
    }
}
