using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.GUI.DocumentWindows;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Actions
{
    public class CreateNewDocumentAction : AbstractUiAction
    {
        private readonly IDockingService _dockingService;

        public CreateNewDocumentAction()
        {
            _dockingService = DependencyFactory.Resolve<IDockingService>();
        }

        public override void Execute()
        {
            Editor doc = new Editor
            {
                Text = "New Document",
                TabText = "New Document"
            };
            doc.TabText = "New Document";
            _dockingService.ShowDockPanel(doc);
        }
    }

    public class OpenFileAction : AbstractUiAction
    {
        private IEnumerable<FilePath> _fileNames;
        private readonly IFileTypeMethodFactory _fileTypeMethodFactory;

        public OpenFileAction() : this(null)
        {
        }

        public OpenFileAction(params FilePath[] fileNames)
        {
            _fileNames = fileNames;
            _fileTypeMethodFactory = DependencyFactory.Resolve<IFileTypeMethodFactory>();
        }

        public override void Execute()
        {
            if (_fileNames == null)
            {
                if (!ShowDialog())
                {
                    return;
                }

                if (_fileNames == null)
                {
                    return;
                }
            }

            foreach (var fileName in _fileNames)
            {
                try
                {
                    if (!_fileTypeMethodFactory.OpenRegisteredFile(fileName))
                    {
                        throw new Exception("Opening file failed");
                    }
                }
                catch (Exception ex)
                {
                    DockingService.ShowError("Error opening file", ex);
                }
            }
        }

        private bool ShowDialog()
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
                return false;
            }

            _fileNames = openFileDialog.FileNames.Select(path => new FilePath(path));
            return true;
        }
    }

    public class OpenProjectCommand : AbstractUiAction
    {
        private readonly IProjectService _projectService;

        public OpenProjectCommand()
        {
            _projectService = DependencyFactory.Resolve<IProjectService>();
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

                FilePath fileName = new FilePath(openFileDialog.FileName);

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
            _editor = DependencyFactory.Resolve<IDockingService>().ActiveDocument as AbstractFileEditor;
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
            _editor = DependencyFactory.Resolve<IDockingService>().ActiveDocument as AbstractFileEditor;
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

            _editor.SaveFile(new FilePath(saveFileDialog.FileName));
        }
    }

    public class SaveAllCommand : AbstractUiAction
    {
        private readonly IDockingService _dockingService;

        public SaveAllCommand()
        {
            _dockingService = DependencyFactory.Resolve<IDockingService>();
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
            _editor = DependencyFactory.Resolve<IDockingService>().ActiveDocument as AbstractFileEditor;
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

    public class DragDropCommand : AbstractUiAction
    {
        private readonly IDataObject _dataObject;

        public DragDropCommand(IDataObject dataObject)
        {
            _dataObject = dataObject;
        }

        public override void Execute()
        {
            if (_dataObject.GetDataPresent(DataFormats.FileDrop) == false)
            {
                return;
            }

            string[] files = (string[]) _dataObject.GetData(DataFormats.FileDrop);
            new OpenFileAction(files.Select(p => new FilePath(p)).ToArray()).Execute();
        }
    }
}