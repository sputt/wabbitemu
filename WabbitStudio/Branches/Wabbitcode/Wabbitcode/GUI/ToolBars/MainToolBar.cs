using System;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.TextEditor.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.ToolBars
{
    public sealed class MainToolBar : ToolStrip
    {
        private static readonly ComponentResourceManager Resources = new ComponentResourceManager(typeof(MainToolBar));

        private readonly ToolStripButton _newToolStripButton = new ToolStripButton
        {
            Text = "&New",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image) (Resources.GetObject("newToolStripButton.Image")))
        };

        private readonly ToolStripButton _openToolStripButton = new ToolStripButton
        {
            Text = "&Open",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image) (Resources.GetObject("openToolStripButton.Image")))
        };

        private readonly ToolStripButton _saveToolStripButton = new ToolStripButton
        {
            Text = "&Save",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image) (Resources.GetObject("saveToolStripButton.Image"))),
            Enabled = false
        };

        private readonly ToolStripButton _saveAllToolButton = new ToolStripButton
        {
            Text = "Save All",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image) (Resources.GetObject("saveAllToolButton.Image"))),
            Enabled = false
        };

        private readonly ToolStripButton _printToolStripButton = new ToolStripButton
        {
            Text = "&Print",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image) (Resources.GetObject("printToolStripButton.Image"))),
            Enabled = false
        };

        private readonly ToolStripSeparator _toolStripSeparator = new ToolStripSeparator();

        private readonly ToolStripButton _cutToolStripButton = new ToolStripButton
        {
            Text = "C&ut",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image) (Resources.GetObject("cutToolStripButton.Image"))),
            Enabled = false
        };

        private readonly ToolStripButton _copyToolStripButton = new ToolStripButton
        {
            Text = "&Copy",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image) (Resources.GetObject("copyToolStripButton.Image"))),
            Enabled = false
        };

        private readonly ToolStripButton _pasteToolStripButton = new ToolStripButton
        {
            Text = "&Paste",
            DisplayStyle = ToolStripItemDisplayStyle.Image,
            Image = ((Image) (Resources.GetObject("pasteToolStripButton.Image"))),
            Enabled = false
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
            Image = ((Image) (Resources.GetObject("runToolButton.Image"))),
            Text = "Start Debug"
        };

        private readonly ToolStripComboBox _configBox = new ToolStripComboBox
        {
            AutoCompleteSource = AutoCompleteSource.CustomSource,
            FlatStyle = FlatStyle.Flat,
        };

        private readonly IDockingService _dockingService;
        private readonly IFileService _fileService;
        private readonly IProjectService _projectService;

        public MainToolBar()
        {
            AllowItemReorder = true;
            Items.AddRange(new ToolStripItem[]
            {
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
                _configBox
            });
            Dock = DockStyle.Left;
            RenderMode = ToolStripRenderMode.System;
            GripStyle = ToolStripGripStyle.Hidden;
            Text = "Main Toolbar";

            _dockingService = DependencyFactory.Resolve<IDockingService>();
            _fileService = DependencyFactory.Resolve<IFileService>();
            _projectService = DependencyFactory.Resolve<IProjectService>();

            _newToolStripButton.Click += newToolButton_Click;
            _openToolStripButton.Click += openToolButton_Click;
            _saveToolStripButton.Click += saveToolButton_Click;
            _saveAllToolButton.Click += saveAllToolButton_Click;
            _cutToolStripButton.Click += cutToolButton_Click;
            _copyToolStripButton.Click += copyToolButton_Click;
            _pasteToolStripButton.Click += pasteToolButton_Click;
            _runToolButton.Click += RunToolButton_Click;
            _configBox.SelectedIndexChanged += configBox_SelectedIndexChanged;
            _findBox.KeyPress += findBox_KeyPress;
            _dockingService.ActiveDocumentChanged += DockingServiceActiveDocumentChanged;
            _projectService.ProjectOpened += ProjectService_ProjectOpened;
        }

        private void configBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            _projectService.Project.BuildSystem.CurrentConfigIndex = _configBox.SelectedIndex;
        }

        private void DockingServiceActiveDocumentChanged(object sender, EventArgs e)
        {
            bool enabled = _dockingService.Documents.Any();
            _saveToolStripButton.Enabled = enabled;
            _saveAllToolButton.Enabled = enabled;
            _cutToolStripButton.Enabled = enabled;
            _copyToolStripButton.Enabled = enabled;
            _pasteToolStripButton.Enabled = enabled;
            _findBox.Enabled = enabled;
        }

        private void ProjectService_ProjectOpened(object sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                this.BeginInvoke(() => ProjectService_ProjectOpened(sender, e));
                return;
            }

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

        private static void newToolButton_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new CreateNewDocumentAction());
        }

        private static void openToolButton_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new OpenFileAction());
        }

        private static void saveToolButton_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new SaveCommand());
        }

        private static void saveAllToolButton_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new SaveAllCommand());
        }

        private static void cutToolButton_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new CutAction());
        }

        private static void copyToolButton_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new CopyAction());
        }

        private static void pasteToolButton_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new PasteAction());
        }

        private static void RunToolButton_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new StartDebuggerAction());
        }

        private void findBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar != (char) Keys.Enter)
            {
                return;
            }

            e.Handled = true;
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

            string fileText = _fileService.GetFileText(editor.FileName);
            TextEditorSearcher searcher = new TextEditorSearcher(fileText);
            int beginOffset = editor.CaretOffset + lookFor.Length + 1;
            bool looped;
            searcher.LookFor = lookFor;
            TextRange range = searcher.FindNext(beginOffset, false, out looped);
            if (range != null)
            {
                editor.SetSelection(range.Offset, range.Length);
                editor.CaretOffset = range.Offset;
            }
            else
            {
                MessageBox.Show("Text not found");
            }
        }
    }
}