using System;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;
using IFileReaderService = Revsoft.Wabbitcode.Services.Interfaces.IFileReaderService;

namespace Revsoft.Wabbitcode.GUI
{
    public sealed class MainToolBar : ToolStrip
    {
        private static readonly ComponentResourceManager Resources = new ComponentResourceManager(typeof(MainToolBar));
        private readonly ToolStripButton _newToolStripButton = new ToolStripButton
        {
            Text = "&New",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image)(Resources.GetObject("newToolStripButton.Image")))

        };
        private readonly ToolStripButton _openToolStripButton = new ToolStripButton
        {
            Text = "&Open",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image)(Resources.GetObject("openToolStripButton.Image")))
        };
        private readonly ToolStripButton _saveToolStripButton = new ToolStripButton
        {
            Text = "&Save",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image)(Resources.GetObject("saveToolStripButton.Image")))
        };
        private readonly ToolStripButton _saveAllToolButton = new ToolStripButton
        {
            Text = "Save All",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image)(Resources.GetObject("saveAllToolButton.Image")))
        };
        private readonly ToolStripButton _printToolStripButton = new ToolStripButton
        {
            Text = "&Print",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Enabled = false,
            Image = ((Image)(Resources.GetObject("printToolStripButton.Image")))
        };
        private readonly ToolStripSeparator _toolStripSeparator = new ToolStripSeparator();
        private readonly ToolStripButton _cutToolStripButton = new ToolStripButton
        {
            Text = "C&ut",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image)(Resources.GetObject("cutToolStripButton.Image")))
        };
        private readonly ToolStripButton _copyToolStripButton = new ToolStripButton
        {
            Text = "&Copy",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image)(Resources.GetObject("copyToolStripButton.Image")))
        };
        private readonly ToolStripButton _pasteToolStripButton = new ToolStripButton
        {
            Text = "&Paste",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image)(Resources.GetObject("pasteToolStripButton.Image")))
        };
        private readonly ToolStripSeparator _toolStripSeparator3 = new ToolStripSeparator();
        private readonly ToolStripComboBox _findBox = new ToolStripComboBox
        {
            AutoCompleteSource = AutoCompleteSource.CustomSource,
            FlatStyle = FlatStyle.Flat
        };
        private readonly ToolStripSeparator _toolStripSeparator2 = new ToolStripSeparator();
        private readonly ToolStripButton _runToolButton = new ToolStripButton
        {
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image)(Resources.GetObject("runToolButton.Image"))),
            Text = "Start Debug"
        };
        private readonly ToolStripComboBox _configBox = new ToolStripComboBox
        {
            AutoCompleteSource = AutoCompleteSource.CustomSource,
            FlatStyle = FlatStyle.Flat,
        };

        private IDockingService _dockingService;
        private IDocumentService _documentService;
        private IFileReaderService _fileReaderService;
        private IProjectService _projectService;

        public MainToolBar()
        {
            AllowItemReorder = true;
            Items.AddRange(new ToolStripItem[] {
            _newToolStripButton,
            _openToolStripButton,
            _saveToolStripButton,
            _saveAllToolButton,
            _printToolStripButton,
            _toolStripSeparator,
            _cutToolStripButton,
            _copyToolStripButton,
            _pasteToolStripButton,
            _toolStripSeparator3,
            _findBox,
            _toolStripSeparator2,
            _runToolButton,
            _configBox});
            RenderMode = ToolStripRenderMode.System;
            GripStyle = ToolStripGripStyle.Hidden;
            Text = "Main Toolbar";

            ServiceFactory.Instance.OnServiceInitialized += Instance_OnServiceInitialized;
            _newToolStripButton.Click += newToolButton_Click;
            _openToolStripButton.Click += openToolButton_Click;
            _saveToolStripButton.Click += saveToolButton_Click;
            _saveAllToolButton.Click += saveAllToolButton_Click;
            _cutToolStripButton.Click += cutToolButton_Click;
            _copyToolStripButton.Click += copyToolButton_Click;
            _pasteToolStripButton.Click += pasteToolButton_Click;
            _configBox.SelectedIndexChanged += configBox_SelectedIndexChanged;
            _findBox.KeyPress += findBox_KeyPress;
        }

        private void configBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            _projectService.Project.BuildSystem.CurrentConfigIndex = _configBox.SelectedIndex;
        }

        void Instance_OnServiceInitialized(object sender, ServiceInitializedEventArgs e)
        {
            if (e.ServiceType == typeof (IProjectService))
            {
                _projectService = (IProjectService) e.Service;
                _projectService.ProjectOpened += _projectService_ProjectOpened;
            }
            else if (e.ServiceType == typeof (IDockingService))
            {
                _dockingService = (IDockingService) e.Service;
                _dockingService.OnActiveDocumentChanged += DockingService_OnActiveDocumentChanged;
            }
            else if (e.ServiceType == typeof (IDocumentService))
            {
                _documentService = (IDocumentService) e.Service;
            }
            else if (e.ServiceType == typeof (IFileReaderService))
            {
                _fileReaderService = (IFileReaderService) e.Service;
            }
        }

        void DockingService_OnActiveDocumentChanged(object sender, EventArgs e)
        {
            bool enabled = _dockingService.Documents.Any();
            _saveToolStripButton.Enabled = enabled;
			_saveAllToolButton.Enabled = enabled;
			_cutToolStripButton.Enabled = enabled;
			_copyToolStripButton.Enabled = enabled;
			_pasteToolStripButton.Enabled = enabled;
            _findBox.Enabled = enabled;
        }

        void _projectService_ProjectOpened(object sender, EventArgs e)
        {
			IProject project = _projectService.Project;
			if (project.IsInternal)
			{
				return;
			}

			foreach (var config in _projectService.Project.BuildSystem.BuildConfigs)
			{
				_configBox.Items.Add(config);
			}

			_configBox.SelectedIndex = _projectService.Project.BuildSystem.CurrentConfigIndex;
        }

        private void newToolButton_Click(object sender, EventArgs e)
        {
            RunCommand(new CreateNewDocumentAction(_dockingService, _documentService));
        }

        private static void openToolButton_Click(object sender, EventArgs e)
        {
            RunCommand(new OpenFileAction());
        }

        private void saveToolButton_Click(object sender, EventArgs e)
        {
            RunCommand(new SaveCommand(_dockingService.ActiveDocument as AbstractFileEditor));
        }

        private static void saveAllToolButton_Click(object sender, EventArgs e)
        {
            RunCommand(new SaveAllCommand());
        }

        private void cutToolButton_Click(object sender, EventArgs e)
        {
            RunCommand(new CutAction(_dockingService.ActiveDocument as IClipboardOperation));
        }

        private void copyToolButton_Click(object sender, EventArgs e)
        {
            RunCommand(new CopyAction(_dockingService.ActiveDocument as IClipboardOperation));
        }

        private void pasteToolButton_Click(object sender, EventArgs e)
        {
            RunCommand(new PasteAction(_dockingService.ActiveDocument as IClipboardOperation));
        }

        private static void RunCommand(AbstractUiAction action)
        {
            action.Execute();
        }

        private void findBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar != (char)Keys.Enter)
            {
                return;
            }

            string lookFor = _findBox.Text;
            if (!_findBox.Items.Contains(lookFor))
            {
                _findBox.Items.Add(lookFor);
            }

            ITextEditor editor = _dockingService.ActiveDocument as ITextEditor;
            if (editor == null)
            {
                return;
            }

            string fileText = _fileReaderService.GetFileText(editor.FileName);
            TextEditorSearcher searcher = new TextEditorSearcher(fileText);
            int beginOffset = editor.CaretOffset;
            bool looped;
            searcher.LookFor = lookFor;
            TextRange range = searcher.FindNext(beginOffset, false, out looped);
            if (range != null)
            {
                editor.SetSelection(range.Offset, range.Length);
            }
            else
            {
                MessageBox.Show("Text not found");
            }
        }
    }
}
