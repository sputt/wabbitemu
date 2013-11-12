using System;
using System.IO;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Actions
{
    public class CreateNewDocumentAction : AbstractUiAction
    {
        private readonly IDocumentService _documentService;
        private readonly IDockingService _dockingService;

        public CreateNewDocumentAction(IDockingService dockingService, IDocumentService documentService)
        {
            _dockingService = dockingService;
            _documentService = documentService;
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
        private readonly IDocumentService _documentService;
        private readonly IProjectService _projectService;

        public OpenFileAction(IDocumentService documentService, IProjectService projectService)
        {
            _documentService = documentService;
            _projectService = projectService;
        }

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
                // TODO: generalize this for all file types
                foreach (var fileName in openFileDialog.FileNames)
                {
                    string extCheck = Path.GetExtension(fileName);
                    if (string.Equals(extCheck, ".wcodeproj", StringComparison.OrdinalIgnoreCase))
                    {
                        _projectService.OpenProject(fileName);
                        if (Settings.Default.StartupProject == fileName)
                        {
                            continue;
                        }

                        if (MessageBox.Show("Would you like to make this your default project?",
                                "Startup Project",
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Question) == DialogResult.Yes)
                        {
                            Settings.Default.StartupProject = fileName;
                        }
                    }
                    else
                    {
                        _documentService.OpenDocument(fileName);
                    }
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

        public OpenProjectCommand(IProjectService projectService)
        {
            _projectService = projectService;
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
        private readonly Editor _editor;

        public SaveCommand(Editor editor)
        {
            _editor = editor;
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
        private readonly Editor _editor;

        public SaveAsCommand(Editor editor)
        {
            _editor = editor;
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

        public SaveAllCommand(IDockingService dockingService)
        {
            _dockingService = dockingService;
        }

        public override void Execute()
        {
            foreach (Editor child in _dockingService.Documents)
            {
                child.SaveFile();
            }
        }
    }

    public class CloseCommand : AbstractUiAction
    {
        private readonly Editor _editor;

        public CloseCommand(Editor editor)
        {
            _editor = editor;
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
