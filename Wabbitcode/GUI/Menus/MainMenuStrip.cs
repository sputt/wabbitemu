using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Drawing;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using Revsoft.Wabbitcode.GUI.DocumentWindows;
using Revsoft.Wabbitcode.Interfaces;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.GUI.Menus
{
    internal sealed class MainMenuStrip : MenuStrip
    {
        private const Image NoIcon = null;

        #region File Menu Items

        private static readonly ToolStripMenuItem NewFileMenuItem = 
            new ToolStripMenuItem("File", NoIcon, newFileMenuItem_Click)
        {
            ShortcutKeys = Keys.Control | Keys.N
        };

        private static readonly ToolStripMenuItem NewProjectMenuItem =
            new ToolStripMenuItem("Project", NoIcon, newProjectMenuItem_Click)
        {
            ShortcutKeys = Keys.Control | Keys.Shift | Keys.N
        };

        private static readonly ToolStripMenuItem NewMenuItem = new ToolStripMenuItem("New", NoIcon,
            new ToolStripItem[] {NewFileMenuItem, NewProjectMenuItem});

        private static readonly ToolStripMenuItem OpenFileMenuItem = 
            new ToolStripMenuItem("File", NoIcon, openFileMenuItem_Click)
        {
            ShortcutKeys = Keys.Control | Keys.O
        };

        private static readonly ToolStripMenuItem OpenProjectMenuItem = 
            new ToolStripMenuItem("Project", NoIcon, openProjectMenuItem_Click)
        {
            ShortcutKeys = Keys.Control | Keys.Shift | Keys.O
        };

        private static readonly ToolStripMenuItem OpenMenuItem = new ToolStripMenuItem("Open", NoIcon,
            new ToolStripItem[] {OpenFileMenuItem, OpenProjectMenuItem});

        private static readonly ToolStripMenuItem SaveMenuItem =
            new ToolStripMenuItem("Save", NoIcon, saveMenuItem_Click)
        {
            ShortcutKeys = Keys.Control | Keys.S
        };

        private static readonly ToolStripMenuItem SaveAsMenuItem = 
            new ToolStripMenuItem("Save As...", NoIcon, saveAsMenuItem_Click);

        private static readonly ToolStripMenuItem SaveAllMenuItem =
            new ToolStripMenuItem("Save All", NoIcon, saveAllMenuItem_Click)
        {
            ShortcutKeys = Keys.Control | Keys.Shift | Keys.S
        };

        private static readonly ToolStripMenuItem SaveProjectMenuItem =
            new ToolStripMenuItem("Save Project", NoIcon, saveProjectMenuItem_Click);

        private static readonly ToolStripMenuItem CloseMenuItem =
            new ToolStripMenuItem("Close", NoIcon, closeMenuItem_Click)
        {
            ShortcutKeys = Keys.Control | Keys.W
        };

        private static readonly ToolStripMenuItem PrintMenuItem = new ToolStripMenuItem("Print")
        {
            Enabled = false,
            ShortcutKeys = Keys.Control | Keys.P
        };

        private static readonly ToolStripMenuItem PrintPreviewMenuItem = new ToolStripMenuItem("Print Preview")
        {
            Enabled = false
        };

        private static readonly ToolStripMenuItem PageSetupMenuItem = new ToolStripMenuItem("Page Setup")
        {
            Enabled = false
        };

        private static readonly ToolStripMenuItem RecentFilesMenuItem = new ToolStripMenuItem("Recent Files");
        private static readonly ToolStripMenuItem ExitMenuItem =
            new ToolStripMenuItem("Exit", NoIcon, exitMenuItem_Click);

        #endregion

        #region Edit Menu Items

        private static readonly ToolStripMenuItem UndoMenuItem = new ToolStripMenuItem("Undo")
        {
            ShortcutKeys = Keys.Control | Keys.Z
        };

        private static readonly ToolStripMenuItem RedoMenuItem = new ToolStripMenuItem("Redo")
        {
            ShortcutKeys = Keys.Control | Keys.Y
        };

        private static readonly ToolStripMenuItem CutMenuItem = new ToolStripMenuItem("Cut")
        {
            ShortcutKeys = Keys.Control | Keys.X
        };

        private static readonly ToolStripMenuItem CopyMenuItem = new ToolStripMenuItem("Copy")
        {
            ShortcutKeys = Keys.Control | Keys.C
        };

        private static readonly ToolStripMenuItem PasteMenuItem = new ToolStripMenuItem("Paste")
        {
            ShortcutKeys = Keys.Control | Keys.V
        };

        private static readonly ToolStripMenuItem SelectAllMenuItem = new ToolStripMenuItem("Select All")
        {
            ShortcutKeys = Keys.Control | Keys.A
        };

        private static readonly ToolStripMenuItem FindMenuItem = new ToolStripMenuItem("Find")
        {
            ShortcutKeys = Keys.Control | Keys.F
        };

        private static readonly ToolStripMenuItem ReplaceMenuItem = new ToolStripMenuItem("Replace")
        {
            ShortcutKeys = Keys.Control | Keys.H
        };

        private static readonly ToolStripMenuItem FindInFilesMenuItem = new ToolStripMenuItem("Find in Files")
        {
            ShortcutKeys = Keys.Control | Keys.Shift | Keys.F
        };

        private static readonly ToolStripMenuItem ReplaceInFilesMenuItem = new ToolStripMenuItem("Replace in Files")
        {
            ShortcutKeys = Keys.Control | Keys.Shift | Keys.H
        };

        private static readonly ToolStripMenuItem FindAllRefsMenuItem = new ToolStripMenuItem("Find All References");

        private static readonly ToolStripMenuItem FinderMenuItem = new ToolStripMenuItem("Find and Replace", null,
            new ToolStripItem[] {FindMenuItem, ReplaceMenuItem, FindInFilesMenuItem, ReplaceInFilesMenuItem, FindAllRefsMenuItem});

        private static readonly ToolStripMenuItem SpriteMenuItem = new ToolStripMenuItem("Sprite")
        {
            Enabled = false
        };

        private static readonly ToolStripMenuItem MapMenuItem = new ToolStripMenuItem("Map")
        {
            Enabled = false
        };

        private static readonly ToolStripMenuItem InsertMenuItem = new ToolStripMenuItem("Insert", null,
            new ToolStripItem[] {SpriteMenuItem, MapMenuItem});

        private static readonly ToolStripMenuItem MakeUpperMenuItem = new ToolStripMenuItem("Make Uppercase")
        {
            ShortcutKeys = Keys.Control | Keys.Shift | Keys.U
        };

        private static readonly ToolStripMenuItem MakeLowerMenuItem = new ToolStripMenuItem("Make Lowercase")
        {
            ShortcutKeys = Keys.Control | Keys.Shift | Keys.L
        };

        private static readonly ToolStripMenuItem InvertCaseMenuItem = new ToolStripMenuItem("Invert Case");
        private static readonly ToolStripMenuItem SentenceCaseMenuItem = new ToolStripMenuItem("Sentence Case");
        private static readonly ToolStripSeparator Separator7 = new ToolStripSeparator();
        private static readonly ToolStripMenuItem FormatDocMenuItem = new ToolStripMenuItem("Format Document");
        private static readonly ToolStripMenuItem ConvertSpacesToTabsMenuItem = new ToolStripMenuItem("Convert Spaces to Tabs");

        private static readonly ToolStripMenuItem FormattingMenuItem = new ToolStripMenuItem("Formatting", null,
            new ToolStripItem[]
            {
                MakeUpperMenuItem, MakeLowerMenuItem, InvertCaseMenuItem, SentenceCaseMenuItem,
                Separator7, FormatDocMenuItem, ConvertSpacesToTabsMenuItem
            });

        private static readonly ToolStripMenuItem ToggleBookmarkMenuItem = new ToolStripMenuItem("Toggle Bookmark");
        private static readonly ToolStripMenuItem PrevBookmarkMenuItem = new ToolStripMenuItem("Goto Previous Bookmark");
        private static readonly ToolStripMenuItem NextBookmarkMenuItem = new ToolStripMenuItem("Goto Next Bookmark");

        private static readonly ToolStripMenuItem BookmarkMenuItem = new ToolStripMenuItem("Bookmarks", null,
            new ToolStripItem[] {ToggleBookmarkMenuItem, PrevBookmarkMenuItem, NextBookmarkMenuItem});

        private static readonly ToolStripMenuItem GLineMenuItem = new ToolStripMenuItem("Line")
        {
            ShortcutKeys = Keys.Control | Keys.G
        };

        private static readonly ToolStripMenuItem GSymbolMenuItem = new ToolStripMenuItem("Symbol")
        {
            ShortcutKeys = Keys.Control | Keys.T
        };

        private static readonly ToolStripMenuItem GotoMenuItem = new ToolStripMenuItem("Goto", null,
            new ToolStripItem[] {GLineMenuItem, GSymbolMenuItem});

        private static readonly ToolStripMenuItem PrefsMenuItem = new ToolStripMenuItem("Preferences");

        #endregion

        #region View Menu Items

        private static readonly ToolStripMenuItem ToolBarMenuItem = new ToolStripMenuItem("Toolbars");

        private static readonly ToolStripMenuItem LabelListMenuItem = new ToolStripMenuItem("Label List")
        {
            CheckOnClick = true,
            Tag = typeof(LabelList)
        };

        private static readonly ToolStripMenuItem ProjViewMenuItem = new ToolStripMenuItem("Project Viewer")
        {
            CheckOnClick = true,
            Tag = typeof(ProjectViewer)
        };

        private static readonly ToolStripMenuItem MacroManagerMenuItem = new ToolStripMenuItem("Macro Manager")
        {
            CheckOnClick = true,
            Tag = typeof(MacroManager)
        };

        private static readonly ToolStripMenuItem DebugPanelMenuItem = new ToolStripMenuItem("Debug Panel")
        {
            CheckOnClick = true,
            Tag = typeof(DebugPanel)
        };

        private static readonly ToolStripMenuItem CallStackMenuItem = new ToolStripMenuItem("Call Stack")
        {
            CheckOnClick = true,
            Tag = typeof(CallStack)
        };

        private static readonly ToolStripMenuItem StackViewerMenuItem = new ToolStripMenuItem("Stack Viewer")
        {
            CheckOnClick = true,
            Tag = typeof(StackViewer)
        };

        private static readonly ToolStripMenuItem VarTrackMenuItem = new ToolStripMenuItem("Watches")
        {
            CheckOnClick = true,
            Tag = typeof(TrackingWindow)
        };

        private static readonly ToolStripMenuItem BreakManagerMenuItem = new ToolStripMenuItem("Breakpoints")
        {
            CheckOnClick = true,
            Tag = typeof(BreakpointManagerWindow)
        };

        private static readonly ToolStripMenuItem DebugPanelsMenuItem = new ToolStripMenuItem("Debug", null,
            new ToolStripItem[]
            {
                DebugPanelMenuItem, CallStackMenuItem, StackViewerMenuItem,
                VarTrackMenuItem, BreakManagerMenuItem
            });

        private static readonly ToolStripMenuItem OutputWindowMenuItem = new ToolStripMenuItem("Output Window")
        {
            CheckOnClick = true,
            Tag = typeof(OutputWindow)
        };

        private static readonly ToolStripMenuItem ErrorListMenuItem = new ToolStripMenuItem("Error List")
        {
            CheckOnClick = true,
            Tag = typeof(ErrorList)
        };

        private static readonly ToolStripMenuItem FindResultsMenuItem = new ToolStripMenuItem("Find Results")
        {
            CheckOnClick = true,
            Tag = typeof(FindResultsWindow)
        };

        private static readonly ToolStripMenuItem StatusBarMenuItem = new ToolStripMenuItem("Status Bar")
        {
            CheckOnClick = true
        };

        #endregion

        #region Refactor Menu Items

        private static readonly ToolStripMenuItem RenameMenuItem = new ToolStripMenuItem("Rename")
        {
            ShortcutKeys = Keys.Control | Keys.R
        };

        private static readonly ToolStripMenuItem ExtractMethodMenuItem = new ToolStripMenuItem("Extract Method");

        #endregion

        #region Project Menu Items

        private static readonly ToolStripMenuItem AddNewFileMenuItem = new ToolStripMenuItem("Add New File");
        private static readonly ToolStripMenuItem ExistingFileMenuItem = new ToolStripMenuItem("Add Existing File");
        private static readonly ToolStripMenuItem BuildOrderButton = new ToolStripMenuItem("Build Order");
        private static readonly ToolStripMenuItem IncludeDirProjMenuItem = new ToolStripMenuItem("Include Directories");
        private static readonly ToolStripMenuItem CloseProjMenuItem = new ToolStripMenuItem("Close Project");

        #endregion

        #region Build Menu Items

        private static readonly ToolStripMenuItem BuildProjectMenuItem = new ToolStripMenuItem("Build Project")
        {
            ShortcutKeys = Keys.Control | Keys.B
        };

        private static readonly ToolStripMenuItem CleanProjectMenuItem = new ToolStripMenuItem("Clean Project");
        private static readonly ToolStripMenuItem ProjStatsMenuItem = new ToolStripMenuItem("Project Statistics");

        #endregion

        #region Debug Menu Items

        private static readonly ToolStripMenuItem StartDebugMenuItem = new ToolStripMenuItem("Start Debugging")
        {
            ShortcutKeys = Keys.F5
        };

        private static readonly ToolStripMenuItem StartWithoutDebugMenuItem = new ToolStripMenuItem("Start Without Debugging")
        {
            ShortcutKeys = Keys.Control | Keys.F5
        };

        private static readonly ToolStripMenuItem StopDebugMenuItem = new ToolStripMenuItem("Stop Debugging");
        private static readonly ToolStripMenuItem RunMenuItem = new ToolStripMenuItem("Run");

        private static readonly ToolStripMenuItem StepMenuItem = new ToolStripMenuItem("Step")
        {
            ShortcutKeys = Keys.F7
        };

        private static readonly ToolStripMenuItem StepOverMenuItem = new ToolStripMenuItem("Step Over")
        {
            ShortcutKeys = Keys.F8
        };

        private static readonly ToolStripMenuItem StepOutMenuItem = new ToolStripMenuItem("Step Out")
        {
            ShortcutKeys = Keys.Shift | Keys.F8
        };

        private static readonly ToolStripMenuItem NewBreakpointMenuItem = new ToolStripMenuItem("New Breakpoint");
        private static readonly ToolStripMenuItem NewDataBreakpointMenuItem = new ToolStripMenuItem("New Data Breakpoint");

        private static readonly ToolStripMenuItem ToggleBreakpointMenuItem = new ToolStripMenuItem("Toggle Breakpoint")
        {
            ShortcutKeys = Keys.F9
        };

        private static readonly ToolStripMenuItem DeleteAllBreakpointsMenuItem = new ToolStripMenuItem("Delete All Breakpoints");
        private static readonly ToolStripMenuItem DisableAllBreakpointsMenuItem = new ToolStripMenuItem("Disable All Breakpoints");

        private static readonly ToolStripMenuItem BreakpointMenuItem = new ToolStripMenuItem("Breakpoint", null, new ToolStripItem[]
        {
            NewBreakpointMenuItem,
            NewDataBreakpointMenuItem,
            ToggleBreakpointMenuItem,
            DeleteAllBreakpointsMenuItem,
            DisableAllBreakpointsMenuItem
        });

        #endregion

        #region Macros Menu Items

        private static readonly ToolStripMenuItem RunMacroMenuItem = new ToolStripMenuItem("Run Macro");
        private static readonly ToolStripMenuItem RecordMacroMenuItem = new ToolStripMenuItem("Record Macro");
        private static readonly ToolStripMenuItem ManageMacroMenuItem = new ToolStripMenuItem("Manage Macros");

        #endregion

        #region Help Menu Items

        private static readonly ToolStripMenuItem ShowHelpMenuItem = new ToolStripMenuItem("Help")
        {
            Enabled = false
        };

        private static readonly ToolStripMenuItem UpdateMenuItem = new ToolStripMenuItem("Check for Updates");
        private static readonly ToolStripMenuItem AboutMenuItem = new ToolStripMenuItem("About");

        #endregion

        #region MenuItems

        private static readonly ToolStripSeparator Separator1 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator2 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator3 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator4 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator5 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator6 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator8 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator9 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator10 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator11 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator12 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator13 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator14 = new ToolStripSeparator();
        private static readonly ToolStripSeparator Separator15 = new ToolStripSeparator();

        private readonly ToolStripMenuItem _fileMenuItem = new ToolStripMenuItem("File", null, new ToolStripItem[]
        {
            NewMenuItem,
            OpenMenuItem,
            SaveMenuItem,
            SaveAsMenuItem,
            SaveAllMenuItem,
            SaveProjectMenuItem,
            CloseMenuItem,
            Separator1,
            PrintMenuItem,
            PrintPreviewMenuItem,
            PageSetupMenuItem,
            Separator2,
            RecentFilesMenuItem,
            Separator3,
            ExitMenuItem
        });

        private readonly ToolStripMenuItem _editMenuItem = new ToolStripMenuItem("Edit", null, new ToolStripItem[]
        {
            UndoMenuItem,
            RedoMenuItem,
            Separator4,
            CutMenuItem,
            CopyMenuItem,
            PasteMenuItem,
            SelectAllMenuItem,
            Separator5,
            FinderMenuItem,
            Separator6,
            InsertMenuItem,
            FormattingMenuItem,
            BookmarkMenuItem,
            GotoMenuItem,
            Separator8,
            PrefsMenuItem
        });

        private readonly ToolStripMenuItem _viewMenuItem = new ToolStripMenuItem("View", null, new ToolStripItem[]
        {
            ToolBarMenuItem,
            Separator9,
            LabelListMenuItem,
            ProjViewMenuItem,
            MacroManagerMenuItem,
            Separator10,
            DebugPanelsMenuItem,
            Separator11,
            OutputWindowMenuItem,
            ErrorListMenuItem,
            FindResultsMenuItem,
            StatusBarMenuItem
        });

        private readonly ToolStripMenuItem _refactorMenuItem = new ToolStripMenuItem("Refactor", null, new ToolStripItem[]
        {
            RenameMenuItem,
            ExtractMethodMenuItem
        });

        private readonly ToolStripMenuItem _projectMenuItem = new ToolStripMenuItem("Project", null, new ToolStripItem[]
        {
            AddNewFileMenuItem,
            ExistingFileMenuItem,
            Separator12,
            BuildOrderButton,
            IncludeDirProjMenuItem,
            Separator13,
            CloseProjMenuItem
        });

        private readonly ToolStripMenuItem _buildMenuItem = new ToolStripMenuItem("Build", null, new ToolStripItem[]
        {
            BuildProjectMenuItem,
            CleanProjectMenuItem,
            ProjStatsMenuItem
        });

        private readonly ToolStripMenuItem _macrosMenuItem = new ToolStripMenuItem("Macros", null, new ToolStripItem[]
        {
            RunMacroMenuItem,
            RecordMacroMenuItem,
            ManageMacroMenuItem
        });

        private readonly ToolStripMenuItem _debugMenuItem = new ToolStripMenuItem("Debug", null, new ToolStripItem[]
        {
            StartDebugMenuItem,
            StartWithoutDebugMenuItem,
            StopDebugMenuItem,
            Separator14,
            RunMenuItem,
            StepMenuItem,
            StepOverMenuItem,
            StepOutMenuItem,
            Separator15,
            BreakpointMenuItem
        });

        private readonly ToolStripMenuItem _windowMenuItem = new ToolStripMenuItem("Window");

        private readonly ToolStripMenuItem _helpMenuItem = new ToolStripMenuItem("Help", null, new ToolStripItem[]
        {
            ShowHelpMenuItem,
            UpdateMenuItem,
            AboutMenuItem
        });

        #endregion

        private readonly IDebuggerService _debuggerService;
        private readonly IDockingService _dockingService;
        private readonly IProjectService _projectService;
        private readonly IToolBarService _toolBarService;
        private readonly IStatusBarService _statusBarService;

        public MainMenuStrip()
        {
            Items.AddRange(new ToolStripItem[]
            {
                _fileMenuItem,
                _editMenuItem,
                _viewMenuItem,
                _refactorMenuItem,
                _projectMenuItem,
                _buildMenuItem,
                _debugMenuItem,
                _macrosMenuItem,
                _windowMenuItem,
                _helpMenuItem
            });

            _debuggerService = DependencyFactory.Resolve<IDebuggerService>();
            _dockingService = DependencyFactory.Resolve<IDockingService>();
            _projectService = DependencyFactory.Resolve<IProjectService>();
            _statusBarService = DependencyFactory.Resolve<IStatusBarService>();
            _toolBarService = DependencyFactory.Resolve<IToolBarService>();
            EnableDocumentMenus();

            _debuggerService.OnDebuggingStarted += DebuggerService_OnDebuggingStarted;
            _debuggerService.OnDebuggingEnded += DebuggerService_OnDebuggingEnded;
            _dockingService.ActiveDocumentChanged += DockingServiceActiveDocumentChanged;
            _projectService.ProjectOpened += ProjectService_OnProjectOpened;
            _projectService.ProjectClosed += ProjectService_OnProjectClosed;
            _toolBarService.OnToolBarVisibilityChanged += ToolBarService_OnToolBarVisibilityChanged;
            _toolBarService.OnToolbarRegistered += ToolBarService_OnToolbarRegistered;
            ToolWindow.OnDockStateChanged += ToolWindow_OnDockStateChanged;

            UndoMenuItem.Click += undoMenuItem_Click;
            RedoMenuItem.Click += redoMenuItem_Click;
            CutMenuItem.Click += cutMenuItem_Click;
            CopyMenuItem.Click += copyMenuItem_Click;
            PasteMenuItem.Click += pasteMenuItem_Click;
            SelectAllMenuItem.Click += selectAllMenuItem_Click;
            FindMenuItem.Click += findMenuItem_Click;
            ReplaceMenuItem.Click += replaceMenuItem_Click;
            FindInFilesMenuItem.Click += findInFilesMenuItem_Click;
            ReplaceInFilesMenuItem.Click += replaceInFilesMenuItem_Click;
            FindAllRefsMenuItem.Click += findAllRefsMenuItem_Click;
            MakeUpperMenuItem.Click += makeUpperMenuItem_Click;
            MakeLowerMenuItem.Click += makeLowerMenuItem_Click;
            InvertCaseMenuItem.Click += invertCaseMenuItem_Click;
            SentenceCaseMenuItem.Click += sentenceCaseMenuItem_Click;
            FormatDocMenuItem.Click += formatDocMenuItem_Click;
            ToggleBookmarkMenuItem.Click += toggleBookmarkMenuItem_Click;
            NextBookmarkMenuItem.Click += nextBookmarkMenuItem_Click;
            PrevBookmarkMenuItem.Click += prevBookmarkMenuItem_Click;
            ConvertSpacesToTabsMenuItem.Click += convertSpacesToTabsMenuItem_Click;
            GLineMenuItem.Click += gLineMenuItem_Click;
            GSymbolMenuItem.Click += gLabelMenuItem_Click;
            PrefsMenuItem.Click += prefsMenuItem_Click;

            LabelListMenuItem.Click += panelMenuItem_Click;
            ProjViewMenuItem.Click += panelMenuItem_Click;
            MacroManagerMenuItem.Click += panelMenuItem_Click;
            OutputWindowMenuItem.Click += panelMenuItem_Click;
            ErrorListMenuItem.Click += panelMenuItem_Click;
            FindResultsMenuItem.Click += panelMenuItem_Click;

            DebugPanelMenuItem.Click += panelMenuItem_Click;
            CallStackMenuItem.Click += panelMenuItem_Click;
            StackViewerMenuItem.Click += panelMenuItem_Click;
            VarTrackMenuItem.Click += panelMenuItem_Click;
            BreakManagerMenuItem.Click += panelMenuItem_Click;

            StatusBarMenuItem.Click += statusBarMenuItem_Click;

            RenameMenuItem.Click += renameMenuItem_Click;
            ExtractMethodMenuItem.Click += extractMethodMenuItem_Click;

            AddNewFileMenuItem.Click += addNewFileMenuItem_Click;
            ExistingFileMenuItem.Click += existingFileMenuItem_Click;
            BuildOrderButton.Click += buildOrderButton_Click;
            IncludeDirProjMenuItem.Click += includeDirButton_Click;
            CloseProjMenuItem.Click += closeProjMenuItem_Click;

            BuildProjectMenuItem.Click += buildMenuItem_Click;

            StartDebugMenuItem.Click += startDebugMenuItem_Click;
            StartWithoutDebugMenuItem.Click += startWithoutDebugMenuItem_Click;
            StopDebugMenuItem.Click += stopDebugMenuItem_Click;
            RunMenuItem.Click += runMenuItem_Click;
            StepMenuItem.Click += stepMenuItem_Click;
            StepOverMenuItem.Click += stepOverMenuItem_Click;
            StepOutMenuItem.Click += stepOutMenuItem_Click;

            NewBreakpointMenuItem.Click += newBreakpointMenuItem_Click;
            ToggleBreakpointMenuItem.Click += toggleBreakpointMenuItem_Click;


            UpdateMenuItem.Click += updateMenuItem_Click;
            AboutMenuItem.Click += aboutMenuItem_Click;

            Task.Factory.StartNew(() =>
            {
                foreach (string file in GetRecentFiles())
                {
                    string fileCopy = file;
                    this.BeginInvoke(() => AddRecentItem(fileCopy));
                }
            });
        }

        /// <summary>
        /// Updates all the menu items that depend on the type of active document open.
        /// </summary>
        private void EnableDocumentMenus()
        {
            IDockContent activeContent = _dockingService.ActiveContent;
            IDockContent activeDocument = _dockingService.ActiveDocument;

            bool hasActiveDocument = activeDocument != null;
            bool hasActiveFileEditor = (activeDocument as AbstractFileEditor) != null;
            bool hasActiveTextEditor = (activeDocument as ITextEditor) != null;
            bool hasUndoableContent = (activeContent as IUndoable) != null;
            bool hasClipboardActions = (activeContent as IClipboardOperation) != null;

            // File Menu
            SaveMenuItem.Enabled = hasActiveFileEditor;
            SaveAsMenuItem.Enabled = hasActiveFileEditor;
            SaveAllMenuItem.Enabled = hasActiveFileEditor;
            CloseMenuItem.Enabled = hasActiveDocument;

            // Edit Menu
            UndoMenuItem.Enabled = hasUndoableContent;
            RedoMenuItem.Enabled = hasUndoableContent;
            CutMenuItem.Enabled = hasClipboardActions;
            CopyMenuItem.Enabled = hasClipboardActions;
            PasteMenuItem.Enabled = hasClipboardActions;
            SelectAllMenuItem.Enabled = hasActiveTextEditor;
            FindMenuItem.Enabled = hasActiveTextEditor;
            ReplaceMenuItem.Enabled = hasActiveTextEditor;
            MakeLowerMenuItem.Enabled = hasActiveTextEditor;
            MakeUpperMenuItem.Enabled = hasActiveTextEditor;
            InvertCaseMenuItem.Enabled = hasActiveTextEditor;
            SentenceCaseMenuItem.Enabled = hasActiveTextEditor;
            ToggleBookmarkMenuItem.Enabled = hasActiveTextEditor;
            NextBookmarkMenuItem.Enabled = hasActiveTextEditor;
            PrevBookmarkMenuItem.Enabled = hasActiveTextEditor;
            GLineMenuItem.Enabled = hasActiveTextEditor;

            // Refactor Menu
            RenameMenuItem.Enabled = hasActiveTextEditor;
            ExtractMethodMenuItem.Enabled = hasActiveTextEditor;

            ToggleBreakpointMenuItem.Enabled = hasActiveTextEditor;

            // Window Menu
            _windowMenuItem.Enabled = hasActiveDocument;
        }

        #region File Menu

        private static void newFileMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new CreateNewDocumentAction());
        }

        private static void newProjectMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new CreateNewProjectAction());
        }

        private static void openFileMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new OpenFileAction());
        }

        private static void openProjectMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new OpenProjectCommand());
        }

        private static void saveMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new SaveCommand());
        }

        private static void saveAsMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new SaveAsCommand());
        }

        private static void saveAllMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new SaveAllCommand());
        }

        private static void saveProjectMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new SaveProjectAction());
        }

        private static void closeMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new CloseCommand());
        }

        /// <summary>
        /// This opens the recent document clicked in the file menu.
        /// </summary>
        /// <param name="sender">This is the button object. This is casted to get which button was clicked.</param>
        /// <param name="e">Nobody cares about this arg.</param>
        private static void OpenRecentDoc(object sender, EventArgs e)
        {
            MenuItem button = (MenuItem) sender;
            AbstractUiAction.RunCommand(new OpenFileAction(new FilePath(button.Text)));
        }

        private static void AddRecentItem(string file)
        {
            ToolStripMenuItem button = new ToolStripMenuItem(file, null, OpenRecentDoc);
            RecentFilesMenuItem.DropDownItems.Add(button);
        }

        private static IEnumerable<string> GetRecentFiles()
        {
            if (Settings.Default.RecentFiles == null)
            {
                Settings.Default.RecentFiles = new StringCollection();
            }

            return Settings.Default.RecentFiles.Cast<string>().Where(s => !string.IsNullOrEmpty(s));
        }

        private static void exitMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        #endregion

        #region Edit Menu

        private static void undoMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new UndoAction());
        }

        private static void redoMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new RedoAction());
        }

        private static void cutMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new CutAction());
        }

        private static void copyMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new CopyAction());
        }

        private static void pasteMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new PasteAction());
        }

        private static void selectAllMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new SelectAllAction());
        }

        private static void findMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new FindAction());
        }

        private static void findInFilesMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new FindInFilesAction());
        }

        private static void replaceMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new ReplaceAction());
        }

        private static void replaceInFilesMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new ReplaceInFilesAction());
        }

        private static void findAllRefsMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new FindAllReferencesAction());
        }

        private static void makeUpperMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new ToUpperAction());
        }

        private static void makeLowerMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new ToLowerAction());
        }

        private static void invertCaseMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new InvertCaseAction());
        }

        private static void sentenceCaseMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new ToSentenceCaseAction());
        }

        private static void formatDocMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new FormatDocumentAction());
        }

        private static void convertSpacesToTabsMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new ConvertSpacesToTabsAction());
        }

        private static void prevBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new GotoPreviousBookmark());
        }

        private static void nextBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new GotoNextBookmark());
        }

        private void toggleBookmarkMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new ToggleBookmark());
        }

        private static void gLineMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new GotoLineAction());
        }

        private static void gLabelMenuItem_Click(object sender, EventArgs e)
        {
            GotoDefinitionAction.FromDialog();
        }

        private static void prefsMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new EditPreferencesAction());
        }

        #endregion

        #region View Menu

        private void ToolWindow_OnDockStateChanged(object sender, EventArgs e)
        {
            ToolWindow window = sender as ToolWindow;
            if (window == null)
            {
                return;
            }

            var viewToolItems = _viewMenuItem.DropDownItems.OfType<ToolStripMenuItem>();
            var debugViewToolItems = DebugPanelsMenuItem.DropDownItems.OfType<ToolStripMenuItem>();
            var menuItem = viewToolItems.Concat(debugViewToolItems).FirstOrDefault(i =>
            {
                var type = i.Tag as Type;
                return type != null && type.IsInstanceOfType(window);
            });

            if (menuItem == null)
            {
                return;
            }

            menuItem.Checked = window.VisibleState != DockState.Hidden && !window.IsHidden;
        }

        private void DockingServiceActiveDocumentChanged(object sender, EventArgs eventArgs)
        {
            EnableDocumentMenus();
        }

        private void ToolBarService_OnToolbarRegistered(object sender, ToolbarEventArgs e)
        {
            ToolStripMenuItem item = new ToolStripMenuItem(e.ToolBarName)
            {
                CheckOnClick = true
            };

            item.Click += (o, args) =>
            {
                ToolStripMenuItem menuItem = (ToolStripMenuItem) o;

                if (menuItem.Checked)
                {
                    _toolBarService.ShowToolBar(menuItem.Text);
                }
                else
                {
                    _toolBarService.HideToolBar(menuItem.Text);
                }
            };
            ToolBarMenuItem.DropDownItems.Add(item);
        }

        private static void ToolBarService_OnToolBarVisibilityChanged(object sender, ToolbarVisibilityChangedEventArgs e)
        {
            var menuItem = ToolBarMenuItem.DropDownItems.OfType<ToolStripMenuItem>()
                .FirstOrDefault(i => i.Text == e.ToolBarName);
            if (menuItem != null)
            {
                menuItem.Checked = e.Visible;
            }
        }

        private void panelMenuItem_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem menuItem = sender as ToolStripMenuItem;
            if (menuItem == null)
            {
                return;
            }

            Type panelType = menuItem.Tag as Type;
            if (panelType == null)
            {
                throw new InvalidOperationException("Panel item tag does not contain panel type");
            }

            if (menuItem.Checked)
            {
                _dockingService.ShowDockPanel(panelType);
            }
            else
            {
                _dockingService.HideDockPanel(panelType);
            }
        }

        private void statusBarMenuItem_Click(object sender, EventArgs e)
        {
            if (StatusBarMenuItem.Checked)
            {
                _statusBarService.ShowStatusBar();
            }
            else
            {
                _statusBarService.HideStatusBar();
            }
        }

        #endregion

        #region Refactor Menu

        private static void renameMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new RefactorRenameAction());
        }

        private static void extractMethodMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new RefactorExtractMethodAction());
        }

        #endregion

        #region Project Menu

        private void ProjectService_OnProjectClosed(object sender, EventArgs eventArgs)
        {
            UpdateProjectMenuItems(!_projectService.Project.IsInternal);
        }

        private void ProjectService_OnProjectOpened(object sender, EventArgs eventArgs)
        {
            UpdateProjectMenuItems(!_projectService.Project.IsInternal);
        }

        private void UpdateProjectMenuItems(bool showProjectMenu)
        {
            _projectMenuItem.Visible = showProjectMenu;
        }

        private void addNewFileMenuItem_Click(object sender, EventArgs e)
        {
            ProjectViewer projectViewer = _dockingService.GetDockingWindow<ProjectViewer>();
            AbstractUiAction.RunCommand(new AddNewFileAction(projectViewer));
        }

        private void existingFileMenuItem_Click(object sender, EventArgs e)
        {
            ProjectViewer projectViewer = _dockingService.GetDockingWindow<ProjectViewer>();
            AbstractUiAction.RunCommand(new AddExistingFileAction(projectViewer));
        }

        private static void buildOrderButton_Click(object sender, EventArgs e)
        {
            using (BuildSteps build = new BuildSteps())
            {
                build.ShowDialog();
            }
        }

        private static void includeDirButton_Click(object sender, EventArgs e)
        {
            IncludeDir includes = new IncludeDir();
            includes.ShowDialog();
            includes.Dispose();
        }

        private static void closeProjMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new CloseProjectAction());
        }

        #endregion

        #region Assemble Menu

        private static void buildMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new BuildAction());
        }

        #endregion

        #region Debug Menu

        private void DebuggerService_OnDebuggingEnded(object sender, DebuggingEventArgs e)
        {
            EnableDebugMenuItems();
        }

        private void DebuggerService_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            e.Debugger.DebuggerRunningChanged += (o, args) => EnableDebugMenuItems();
            e.Debugger.DebuggerStep += (o, args) => EnableDebugMenuItems();
        }

        private void EnableDebugMenuItems()
        {
            if (InvokeRequired)
            {
                this.Invoke(EnableDebugMenuItems);
                return;
            }

            bool isDebugging = _debuggerService.CurrentDebugger != null;
            bool isRunning = isDebugging && _debuggerService.CurrentDebugger.IsRunning;
            bool enabled = isDebugging && !isRunning;
            bool hasCallStack = isDebugging && _debuggerService.CurrentDebugger.CallStack.Count > 0;
            StepMenuItem.Enabled = enabled;
            StartDebugMenuItem.Enabled = enabled || !isDebugging;
            StepOverMenuItem.Enabled = enabled;
            StepOutMenuItem.Enabled = enabled && hasCallStack;
            StopDebugMenuItem.Enabled = isDebugging;
            RunMenuItem.Enabled = enabled;
        }

        private static void startDebugMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new StartDebuggerAction());
        }

        private static void startWithoutDebugMenuItem_Click(object sender, EventArgs e)
        {
            // TODO: fix this
            //_debugger.StartWithoutDebug();
        }

        private static void stopDebugMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new StopDebuggerAction());
        }

        private static void runMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new StartDebuggerAction());
        }

        private static void stepMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new StepDebuggerAction());
        }

        private static void stepOverMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new StepOverDebuggerAction());
        }

        private static void stepOutMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new StepOutDebuggerAction());
        }

        private static void newBreakpointMenuItem_Click(object sender, EventArgs e)
        {
            NewBreakpointForm form = new NewBreakpointForm();
            form.ShowDialog();
            form.Dispose();
        }

        private static void toggleBreakpointMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new ToggleBreakpointAction());
        }

        #endregion

        #region Help Menu

        private static void updateMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new CheckForUpdateAction());
        }

        private static void aboutMenuItem_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new AboutDialogAction());
        }

        #endregion
    }
}