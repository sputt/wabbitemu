using Microsoft.Win32;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Docking_Windows;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.Exceptions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Services.Symbols;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode
{
	public partial class MainForm : Form
	{
		#region Private Members

		private readonly List<ArrayList> _errorsToAdd = new List<ArrayList>();
		private bool _showToolbar = true;
		private WabbitcodeDebugger _debugger;

		#region Services

		private IDockingService _dockingService;
		private IProjectService _projectService;
		private IAssemblerService _assemblerService;
		private IParserService _parserService;
		private ISymbolService _symbolService;
		private IBackgroundAssemblerService _backgroundAssemblerService;
		private IDocumentService _documentService;

		#endregion


		#endregion

		#region Events
		public delegate void DebuggingStarted(object sender, DebuggingEventArgs e);
		public event DebuggingStarted OnDebuggingStarted;

		public delegate void DebuggingEnded(object sender, DebuggingEventArgs e);
		public event DebuggingEnded OnDebuggingEnded;
		#endregion

		public MainForm(string[] args)
		{
			InitializeComponent();
			RestoreWindow();
			InitiailzeToolbars();
			InitializeService();
			InitializeEvents();

			DockingService.OnActiveDocumentChanged += DockingService_OnActiveDocumentChanged;
		
			WabbitcodeBreakpointManager.OnBreakpointAdded += WabbitcodeBreakpointManager_OnBreakpointAdded;
			WabbitcodeBreakpointManager.OnBreakpointRemoved += WabbitcodeBreakpointManager_OnOnBreakpointRemoved;

			_dockingService.InitPanels(new ProjectViewer(_dockingService, _documentService, _projectService),
				new ErrorList(_assemblerService, _dockingService, _documentService, _projectService),
				new TrackingWindow(_dockingService, _symbolService),
				new DebugPanel(_dockingService, _documentService, _symbolService),
				new CallStack(_dockingService, _symbolService),
				new LabelList(_dockingService, _documentService, _parserService),
				new OutputWindow(_dockingService, _documentService),
				new FindAndReplaceForm(_dockingService, _projectService),
				new FindResultsWindow(_dockingService, _documentService),
				new MacroManager(_dockingService),
				new BreakpointManagerWindow(_dockingService, _documentService, _projectService),
				new StackViewer(_dockingService, _symbolService));
			_dockingService.LoadConfig();

			if (args.Length == 0)
			{
				LoadStartupProject();
			}

			if (_projectService.Project == null)
			{
				_projectService.CreateInternalProject();
			}

			try
			{
				if (!_projectService.Project.IsInternal)
				{
					_projectService.Project.InitWatcher(projectWatcher_Changed, projectWatcher_Renamed);
					_dockingService.ProjectViewer.BuildProjTree();
				}
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error building project tree", ex);
			}

			HandleArgs(args);
			UpdateMenus(_dockingService.ActiveDocument != null);
			UpdateChecks();
			UpdateConfigToolbarBox();

			try
			{
				_documentService.GetRecentFiles();
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error getting recent files", ex);
			}
		}

		private void InitializeEvents()
		{
			Editor.OnEditorSelectionChanged += GetCodeInfo;
		}

		private void InitializeService()
		{
			_dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>(dockPanel);
			_assemblerService = ServiceFactory.Instance.GetServiceInstance<IAssemblerService>(new SpasmExeAssembler());
			_projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
			_parserService = ServiceFactory.Instance.GetServiceInstance<IParserService>();
			_symbolService = ServiceFactory.Instance.GetServiceInstance<ISymbolService>();
			_backgroundAssemblerService = ServiceFactory.Instance.GetServiceInstance<IBackgroundAssemblerService>();
			_documentService = ServiceFactory.Instance.GetServiceInstance<IDocumentService>();
		}

		private void InitiailzeToolbars()
		{
			if (Settings.Default.mainToolBar)
			{
				mainToolBar.Show();
			}
			else
			{
				mainToolBar.Hide();
			}

			if (Settings.Default.debugToolbar)
			{
				debugToolStrip.Show();
			}
			else
			{
				debugToolStrip.Hide();
			}
		}

		private void projectWatcher_Changed(object sender, FileSystemEventArgs e)
		{
			switch (e.ChangeType)
			{
				case WatcherChangeTypes.Changed:
					if (!string.IsNullOrEmpty(Path.GetExtension(e.FullPath)))
					{
						foreach (Action action in _dockingService.Documents
							.Where(doc => string.Equals(doc.FileName, e.FullPath, StringComparison.OrdinalIgnoreCase))
							.Select(tempDoc => (Action)(() =>
							{
								const string modifiedFormat = "{0} modified outside the editor.\nLoad changes?";
								DialogResult result = MessageBox.Show(string.Format(modifiedFormat, e.FullPath),
									"File modified", MessageBoxButtons.YesNo);
								if (result == DialogResult.Yes)
								{
									_documentService.OpenDocument(tempDoc, e.FullPath);
								}
							})))
						{
							Invoke(action);
							break;
						}
					}

					break;
			}
		}

		private void projectWatcher_Renamed(object sender, RenamedEventArgs e)
		{
			if (e.OldFullPath == _projectService.Project.ProjectDirectory)
			{
				if (
					MessageBox.Show("Project Folder was renamed, would you like to rename the project?",
									"Rename project",
									MessageBoxButtons.YesNo,
									MessageBoxIcon.Information) == DialogResult.Yes)
				{
					_projectService.Project.ProjectName = Path.GetFileNameWithoutExtension(e.FullPath);
				}
			}
		}

		private void cancelDebug_Click(object sender, EventArgs e)
		{
			CancelDebug();
		}

		public void MainFormRedone_DragDrop(object sender, DragEventArgs e)
		{
			if (e.Data.GetDataPresent(DataFormats.FileDrop) == false)
			{
				return;
			}

			string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
			foreach (string file in files)
			{
				_documentService.OpenDocument(file);
			}
		}

		public void MainFormRedone_DragEnter(object sender, DragEventArgs e)
		{
			e.Effect = e.Data.GetDataPresent(DataFormats.FileDrop) ? DragDropEffects.Copy : DragDropEffects.None;
		}

		public void ProcessParameters(string[] args)
		{
			// The form has loaded, and initialization will have been be done.
			HandleArgs(args);
			Activate();
		}

		public void UpdateChecks()
		{
			if (IsDisposed || Disposing)
			{
				return;
			}

			mainToolMenuItem.Checked = true; // toolBarManager.ContainsControl(mainToolBar);
			debugToolMenuItem.Checked = false; // toolBarManager.ContainsControl(debugToolStrip);
			labelListMenuItem.Checked = _dockingService.LabelList.Visible;
			projViewMenuItem.Checked = _dockingService.ProjectViewer.Visible;
			findResultsMenuItem.Checked = _dockingService.FindResults.Visible;

			// output stuff
			outWinMenuItem.Checked = _dockingService.OutputWindow.Visible;
			errListMenuItem.Checked = _dockingService.ErrorList.Visible;

			// debug stuff
			breakManagerMenuItem.Checked = _dockingService.BreakManagerWindow.Visible;
			debugPanelMenuItem.Checked = _dockingService.DebugPanel.Visible;
			callStackMenuItem.Checked = _dockingService.CallStack.Visible;
			stackViewerMenuItem.Checked = _dockingService.StackViewer.Visible;
			varTrackMenuItem.Checked = _dockingService.TrackWindow.Visible;

			// misc stuff
			statusBarMenuItem.Checked = statusBar.Visible;
			lineNumMenuItem.Checked = Settings.Default.lineNumbers;
			iconBarMenuItem.Checked = Settings.Default.iconBar;
		}

		private readonly object _codeInfoLock = new object();
		private void GetCodeInfo(object sender, EditorSelectionEventArgs e)
		{
			if (string.IsNullOrEmpty(e.SelectedLines))
			{
				return;
			}

			lock (_codeInfoLock)
			{
				CodeCountInfo info = _assemblerService.CountCode(e.SelectedLines);
				this.Invoke(() => UpdateCodeInfo(info));
			}
		}

		/// <summary>
		/// Updates the code info with the latest size, min, and max string in the status bar.
		/// </summary>
		/// <param name="info">Code count information containing min runtime, max runtime and size</param>
		private void UpdateCodeInfo(CodeCountInfo info)
		{
			lineCodeInfo.Text = string.Format("Min: {0} Max: {1} Size: {2}", info.Min, info.Max, info.Size);
		}

		private void UpdateConfigToolbarBox()
		{
			IProject project = _projectService.Project;
			if (project.IsInternal)
			{
				return;
			}

			foreach (var config in _projectService.Project.BuildSystem.BuildConfigs)
			{
				configBox.Items.Add(config);
			}

			configBox.SelectedIndex = _projectService.Project.BuildSystem.CurrentConfigIndex;
		}

		private void UpdateDebugStuff()
		{
			bool isRunning = _debugger != null && _debugger.IsRunning;
			bool isDebugging = _debugger != null;
			bool enabled = !isRunning && isDebugging;
			stepMenuItem.Enabled = enabled;
			gotoCurrentToolButton.Enabled = enabled;
			stepToolButton.Enabled = enabled;
			startDebugMenuItem.Enabled = enabled;
			stepOverMenuItem.Enabled = enabled;
			stepOverToolButton.Enabled = enabled;
			stepOutMenuItem.Enabled = enabled;
			stepOutToolButton.Enabled = enabled;
			stopDebugMenuItem.Enabled = isDebugging;
			stopToolButton.Enabled = isDebugging;
			runDebuggerToolButton.Enabled = enabled;
			runToolButton.Enabled = enabled;
			pauseToolButton.Enabled = isRunning && isDebugging;
		}

		/// <summary>
		/// Updates the title of the app with the filename.
		/// </summary>
		public void UpdateTitle()
		{
			string debugString = string.Empty;
			if (_debugger != null)
			{
				debugString = " (Debugging)";
			}
			if (!string.IsNullOrEmpty(_documentService.ActiveFileName))
			{
				Text = Path.GetFileName(_documentService.ActiveFileName) + debugString + " - Wabbitcode";
			}
			else
			{
				Text = "Wabbitcode" + debugString;
			}
		}

		internal void AddRecentItem(string file)
		{
			ToolStripMenuItem button = new ToolStripMenuItem(file, null, OpenRecentDoc);
			recentFilesMenuItem.DropDownItems.Add(button);
		}

		private void CancelDebug()
		{
			_debugger.CancelDebug();

			if (InvokeRequired)
			{
				this.Invoke(CancelDebug);
				return;
			}
			UpdateTitle();
			UpdateDebugStuff();
			if (_dockingService.DebugPanel != null)
			{
				_dockingService.HideDockPanel(_dockingService.DebugPanel);
			}

			if (_dockingService.TrackWindow != null)
			{
				_dockingService.HideDockPanel(_dockingService.TrackWindow);
			}

			if (_dockingService.CallStack != null)
			{
				_dockingService.CallStack.Clear();
				_dockingService.HideDockPanel(_dockingService.CallStack);
			}

			Settings.Default.debugToolbar = _showToolbar;
			if (!_showToolbar)
			{
				debugToolStrip.Visible = false;
			}

			UpdateChecks();
			_documentService.RemoveDebugHighlight();
			foreach (Editor child in MdiChildren)
			{
				child.RemoveInvisibleMarkers();
				child.CanSetNextStatement = false;
			}

			_debugger = null;

			UpdateTitle();

			if (OnDebuggingEnded != null)
			{
				OnDebuggingEnded(this, new DebuggingEventArgs(null));
			}
		}

		internal void ClearRecentItems()
		{
			recentFilesMenuItem.DropDownItems.Clear();
		}

		private void DoneStep(object sender, DebuggerStepEventArgs e)
		{
			this.Invoke(() =>
			{
				UpdateStepOut();
				UpdateDebugStuff();
				_documentService.RemoveDebugHighlight();
				_documentService.GotoLine(e.Location.FileName, e.Location.LineNumber);
				_documentService.HighlightDebugLine(e.Location.LineNumber);
				_dockingService.MainForm.UpdateTrackPanel();
				_dockingService.MainForm.UpdateDebugPanel();
			});
		}

		private void BreakpointHit(object sender, DebuggerBreakpointHitEventArgs e)
		{
			this.Invoke(() =>
			{
				_documentService.GotoLine(e.Location.FileName, e.Location.LineNumber);
				_documentService.HighlightDebugLine(e.Location.LineNumber);

				// switch to back to us
				Activate();
				UpdateDebugStuff();
				UpdateTrackPanel();
				UpdateDebugPanel();
			});
		}

		internal void HideProgressBar()
		{
			progressBar.Visible = false;
		}

		internal void SetLineAndColStatus(string line, string col)
		{
			lineStatusLabel.Text = "Ln: " + line;
			colStatusLabel.Text = "Col: " + col;
		}

		internal void SetProgress(int percent)
		{
			progressBar.Visible = true;
			progressBar.Value = percent;
		}

		private void SetToolStripText(string text)
		{
			toolStripStatusLabel.Text = text;
		}

		private void StartDebug()
		{
			if (_projectService.Project.IsInternal)
			{
				throw new DebuggingException("Debugging single files is not supported");
			}

			DoAssembly(null, (sender, e) =>
			{
				if (!e.AssemblySucceeded)
				{
					if (DockingService.ShowMessageBox(_dockingService.MainForm,
									"There were errors assembling. Would you like to continue and try to debug?",
									"Continue",
									MessageBoxButtons.YesNo,
									MessageBoxIcon.Error) == DialogResult.No)
					{
						CancelDebug();
						return;
					}
				}
				_debugger = new WabbitcodeDebugger(_documentService, _symbolService);
				_debugger.OnDebuggerBreakpointHit += BreakpointHit;
				_debugger.OnDebuggerStep += DoneStep;
				_debugger.OnDebuggerRunningChanged += debugger_OnDebuggerRunningChanged;
				_debugger.OnDebuggerClosed += (o, args) => CancelDebug();

				string createdName;
				try
				{
					createdName = _debugger.GetOutputFileDetails(e.Project);
				}
				catch (DebuggingException ex)
				{
					DockingService.ShowError("Unable to start debugging", ex);
					CancelDebug();
					return;
				}

				try
				{
					_debugger.InitDebugger(createdName);
				}
				catch (MissingROMException)
				{
					this.Invoke(() =>
					{
						OpenFileDialog openFileDialog = new OpenFileDialog()
						{
							CheckFileExists = true,
							DefaultExt = "*.rom",
							Filter = "All Know File Types | *.rom; *.sav; |ROM Files (*.rom)|*.rom|" +
							         "Savestate Files (*.sav)|*.sav|All Files(*.*)|*.*",
							FilterIndex = 0,
							Multiselect = true,
							RestoreDirectory = true,
							Title = "Select new ROM file",
						};

						if (openFileDialog.ShowDialog() != DialogResult.OK)
						{
							CancelDebug();
							return;
						}

						SaveRomPathRegistry(openFileDialog.FileName);
					});

					_debugger.InitDebugger(createdName);
				}

				if (OnDebuggingStarted != null)
				{
					OnDebuggingStarted(this, new DebuggingEventArgs(_debugger));
				}

				if (_debugger.IsAnApp)
				{
					try
					{
						_debugger.VerifyApp(createdName);
					}
					catch (DebuggingException)
					{
						if (DockingService.ShowMessageBox(this, "Unable to find the app, would you like to try and continue and debug?",
								"Missing App",
								MessageBoxButtons.YesNo,
								MessageBoxIcon.Exclamation) != DialogResult.Yes)
						{
							CancelDebug();
							return;
						}
					}

					this.Invoke(() =>
					{
						UpdateDebugStuff();
						UpdateBreakpoints();
						ShowDebugPanels();
					});

					_debugger.LaunchApp(createdName);
				}
			});
		}

		private static void SaveRomPathRegistry(string romFileName)
		{
			RegistryKey romKey = null;
			try
			{
				romKey = Registry.CurrentUser.OpenSubKey("Software\\Wabbitemu", true);
				if (romKey != null)
				{
					romKey.SetValue("rom_path", romFileName);
				}
			}
			finally
			{
				if (romKey != null)
				{
					romKey.Close();
				}
			}
		}

		void debugger_OnDebuggerRunningChanged(object sender, DebuggerRunningEventArgs e)
		{
			if (e.Running)
			{
				UpdateDebugStuff();
				_documentService.RemoveDebugHighlight();
				_dockingService.ActiveDocument.Refresh();
			}
			else
			{
				Activate();
				UpdateDebugStuff();
				UpdateTrackPanel();
				UpdateDebugPanel();

				_documentService.GotoLine(e.Location.FileName, e.Location.LineNumber);
				_documentService.HighlightDebugLine(e.Location.LineNumber);
			}
		}

		private void ShowDebugPanels()
		{
			_showToolbar = Settings.Default.debugToolbar;
			Settings.Default.debugToolbar = true;
			if (!_showToolbar)
			{
				debugToolStrip.Visible = true;
			}

			debugToolStrip.Height = mainToolBar.Height;
			UpdateChecks();
			_dockingService.ShowDockPanel(_dockingService.DebugPanel);
			_dockingService.ShowDockPanel(_dockingService.TrackWindow);
			_dockingService.ShowDockPanel(_dockingService.CallStack);
			UpdateTitle();
		}


		private void WabbitcodeBreakpointManager_OnOnBreakpointRemoved(object sender, WabbitcodeBreakpointEventArgs e)
		{
			if (_debugger != null)
			{
				_debugger.ClearBreakpoint(e.Breakpoint);
			}
		}

		void WabbitcodeBreakpointManager_OnBreakpointAdded(object sender, WabbitcodeBreakpointEventArgs e)
		{
			if (_debugger != null)
			{
				_debugger.SetBreakpoint(e.Breakpoint);
			}
		}

		private void UpdateBreakpoints()
		{
			foreach (WabbitcodeBreakpoint breakpoint in WabbitcodeBreakpointManager.Breakpoints)
			{
				WabbitcodeBreakpoint newBreakpoint = breakpoint;
				string fileName = newBreakpoint.File;
				int lineNumber = newBreakpoint.LineNumber;

				CalcLocation value = _symbolService.ListTable.GetCalcLocation(fileName, lineNumber);
				if (WabbitcodeDebugger.FindBreakpoint(newBreakpoint) == null)
				{
					_debugger.AddBreakpoint(lineNumber, fileName);
				}

				if (value != null)
				{
					newBreakpoint.Address = value.Address;
					newBreakpoint.IsRam = newBreakpoint.Address > 0x8000;
					if (_debugger.IsAnApp && !newBreakpoint.IsRam)
					{
						newBreakpoint.Page = (byte)(_debugger.AppPage - value.Page);
					}
					else
					{
						newBreakpoint.Page = value.Page;
					}

					newBreakpoint.File = fileName;
					newBreakpoint.LineNumber = lineNumber;
					_debugger.SetBreakpoint(newBreakpoint);
				}
				else
				{
					Editor openEditor = _dockingService.Documents.SingleOrDefault(d => d.FileName == newBreakpoint.File);
					if (openEditor != null)
					{
						openEditor.RemoveBreakpoint(newBreakpoint.LineNumber);
					}
				}
			}

			foreach (Editor d in _dockingService.Documents)
			{
				d.CanSetNextStatement = true;
			}
		}

		private void UpdateDebugPanel()
		{
			_dockingService.DebugPanel.UpdateFlags();
			_dockingService.DebugPanel.UpdateRegisters();
			_dockingService.DebugPanel.UpdateScreen();
		}

		/// <summary>
		/// Updates all the menu items that depend on if there is an active child open.
		/// </summary>
		/// <param name="enabled">Whether items should be enabled or disabled.</param>
		private void UpdateMenus(bool enabled)
		{
			// Main Toolbar
			saveToolStripButton.Enabled = enabled;
			saveAllToolButton.Enabled = enabled;
			cutToolStripButton.Enabled = enabled;
			copyToolStripButton.Enabled = enabled;
			pasteToolStripButton.Enabled = enabled;
			findBox.Enabled = enabled;

			// File Menu
			saveMenuItem.Enabled = enabled;
			saveAsMenuItem.Enabled = enabled;
			saveAllMenuItem.Enabled = enabled;
			closeMenuItem.Enabled = enabled;

			// Edit Menu
			undoMenuItem.Enabled = enabled;
			redoMenuItem.Enabled = enabled;
			cutMenuItem.Enabled = enabled;
			copyMenuItem.Enabled = enabled;
			pasteMenuItem.Enabled = enabled;
			selectAllMenuItem.Enabled = enabled;
			findMenuItem.Enabled = enabled;
			replaceMenuItem.Enabled = enabled;
			makeLowerMenuItem.Enabled = enabled;
			makeUpperMenuItem.Enabled = enabled;
			invertCaseMenuItem.Enabled = enabled;
			sentenceCaseMenuItem.Enabled = enabled;
			toggleBookmarkMenuItem.Enabled = enabled;
			nextBookmarkMenuItem.Enabled = enabled;
			prevBookmarkMenuItem.Enabled = enabled;
			gLineMenuItem.Enabled = enabled;

			// View Menu
			lineNumMenuItem.Enabled = enabled;
			iconBarMenuItem.Enabled = enabled;

			// Refactor Menu
			renameMenuItem.Enabled = enabled;
			extractMethodMenuItem.Enabled = enabled;

			// Assemble Menu
			symTableMenuItem.Enabled = enabled;
			projStatsMenuItem.Enabled = enabled;
			listFileMenuItem.Enabled = enabled;

			// Debug Menu
			if (!enabled)
			{
				UpdateDebugStuff();
			}

			toggleBreakpointMenuItem.Enabled = enabled;
			if (_projectService.Project.IsInternal)
			{
				startDebugMenuItem.Enabled = enabled;
				startWithoutDebugMenuItem.Enabled = enabled;
				runToolButton.Enabled = enabled;
				runMenuItem.Enabled = enabled;
				runDebuggerToolButton.Enabled = enabled;
				assembleMenuItem.Enabled = enabled;
			}
			else
			{
				startDebugMenuItem.Enabled = true;
				startWithoutDebugMenuItem.Enabled = true;
				runToolButton.Enabled = true;
				runMenuItem.Enabled = true;
				runDebuggerToolButton.Enabled = true;
				assembleMenuItem.Enabled = true;
			}

			// Window Menu
			windowMenuItem.Enabled = enabled;
		}

		private void UpdateProjectMenu(bool projectOpen)
		{
			projMenuItem.Visible = projectOpen;
			includeDirButton.Visible = !projectOpen;
			saveProjectMenuItem.Visible = projectOpen;
		}

		private void UpdateTrackPanel()
		{
			_dockingService.TrackWindow.UpdateVars();
		}

		private void aboutMenuItem_Click(object sender, EventArgs e)
		{
			AboutBox box = new AboutBox();
			box.ShowDialog();
			box.Dispose();
		}

		private void addNewFileMenuItem_Click(object sender, EventArgs e)
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
			_dockingService.ProjectViewer.AddNewFile(name);
		}

		private void AddSquiggleLine(int newLineNumber, Color underlineColor, string description)
		{
			if (_documentService.ActiveDocument == null)
			{
				return;
			}

			_documentService.ActiveDocument.AddSquiggleLine(newLineNumber, underlineColor, description);
		}

		private void DoAssembly(AssemblerService.OnFinishAssemblyFile fileEventHandler, AssemblerService.OnFinishAssemblyProject projectEventHandler)
		{
			_dockingService.OutputWindow.ClearOutput();
			if (!_projectService.Project.IsInternal)
			{
				if (projectEventHandler != null)
				{
					_assemblerService.AssemblerProjectFinished += projectEventHandler;
				}
				Task.Factory.StartNew(() =>
				{
					_assemblerService.AssembleProject(_projectService.Project);
					if (projectEventHandler != null)
					{
						_assemblerService.AssemblerProjectFinished -= projectEventHandler;
					}
				});
			}
			else if (_dockingService.ActiveDocument != null)
			{
				bool saved = _dockingService.ActiveDocument.SaveFile();
				string inputFile = _dockingService.ActiveDocument.FileName;
				if (!saved)
				{
					return;
				}

				if (fileEventHandler != null)
				{
					_assemblerService.AssemblerFileFinished += fileEventHandler;
				}

				Task.Factory.StartNew(() =>
				{
					string outputFile = Path.ChangeExtension(inputFile, _assemblerService.GetExtension(Settings.Default.outputFile));
					string originalDir = Path.GetDirectoryName(inputFile);
					_assemblerService.AssembleFile(inputFile, outputFile, originalDir, Settings.Default.includeDirs.Cast<string>());
					if (fileEventHandler != null)
					{
						_assemblerService.AssemblerFileFinished -= fileEventHandler;
					}
				});
			}
		}

		private void assembleMenuItem_Click(object sender, EventArgs e)
		{
			DoAssembly(OnAssemblyFinished, OnAssemblyFinished);
		}

		private void buildOrderButton_Click(object sender, EventArgs e)
		{
			using (BuildSteps build = new BuildSteps(_projectService.Project))
			{
				build.ShowDialog();
			}
		}

		private void closeMenuItem_Click(object sender, EventArgs e)
		{
			if (ActiveMdiChild != null)
			{
				ActiveMdiChild.Close();
			}
		}

		private void CloseProject()
		{
			DialogResult result = DialogResult.No;
			if (_projectService.Project.NeedsSave && !Settings.Default.autoSaveProject)
			{
				result = MessageBox.Show("Would you like to save your changes to the project file?", "Save project?", MessageBoxButtons.YesNo, MessageBoxIcon.None);
			}
			if (result == DialogResult.Yes || Settings.Default.autoSaveProject)
			{
				_projectService.SaveProject();
			}

			_projectService.CloseProject();
			_dockingService.ProjectViewer.CloseProject();
			UpdateProjectMenu(false);
		}

		private void closeProjMenuItem_Click(object sender, EventArgs e)
		{
			CloseProject();
		}

		private void configBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			_projectService.Project.BuildSystem.CurrentConfigIndex = configBox.SelectedIndex;
		}

		private void convertSpacesToTabsMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.ActiveDocument.ConvertSpacesToTabs();
		}

		private void Copy()
		{
			var activeContent = _dockingService.ActiveContent as ToolWindow;
			if (activeContent != null)
			{
				activeContent.Copy();
			}
			else if (_dockingService.ActiveDocument != null)
			{
				_dockingService.ActiveDocument.Copy();
			}
		}

		private void copyMenuItem_Click(object sender, EventArgs e)
		{
			Copy();
		}

		private void copyToolButton_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument != null)
			{
				_dockingService.ActiveDocument.Copy();
			}
		}

		private void Cut()
		{
			var toolWindow = _dockingService.ActiveContent as ToolWindow;
			if (toolWindow != null)
			{
				toolWindow.Cut();
			}
			else if (_dockingService.ActiveDocument != null)
			{
				_dockingService.ActiveDocument.Cut();
			}
		}

		private void cutMenuItem_Click(object sender, EventArgs e)
		{
			Cut();
		}

		private void cutToolButton_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument != null)
			{
				_dockingService.ActiveDocument.Cut();
			}
		}

		private void DockingService_OnActiveDocumentChanged(object sender, EventArgs eventArgs)
		{
			if (Disposing)
			{
				return;
			}

			if (ActiveMdiChild != null)
			{
				UpdateMenus(true);
				_dockingService.LabelList.UpdateLabelBox();
			}
			else
			{
				UpdateMenus(false);
				_dockingService.LabelList.DisableLabelBox();
			}

			UpdateTitle();
		}

		private void documentParser_DoWork(object sender, DoWorkEventArgs e)
		{
			ArrayList arguments = (ArrayList)e.Argument;
			TextEditorControl editorBox = (TextEditorControl)arguments[0];
			string text = arguments[1].ToString();
			foreach (TextMarker marker in editorBox.Document.MarkerStrategy.TextMarker.Where(marker => marker.Tag == "Code Check"))
			{
				editorBox.Document.MarkerStrategy.RemoveMarker(marker);
			}

			string filePath = editorBox.FileName;

			// setup wabbitspasm to run silently
			Process wabbitspasm = new Process
			{
				StartInfo =
				{
					FileName = FileLocations.SpasmFile,
					RedirectStandardOutput = true,
					RedirectStandardError = true,
					UseShellExecute = false,
					CreateNoWindow = true
				}
			};

			// some strings we'll need to build
			string originaldir = Path.GetDirectoryName(filePath);
			string includedir = "-I \"" + Application.StartupPath + "\"";

			IEnumerable<string> includeDirs = Settings.Default.includeDirs.Cast<string>();
			includedir = includeDirs.Where(dir => !string.IsNullOrEmpty(dir))
				.Aggregate(includedir, (current, dir) => current + (";\"" + dir + "\""));

			wabbitspasm.StartInfo.Arguments = "-V " + includedir + " " + text;
			wabbitspasm.StartInfo.WorkingDirectory = string.IsNullOrEmpty(originaldir) ?
				Application.StartupPath : originaldir;
			wabbitspasm.Start();
			string output = wabbitspasm.StandardOutput.ReadToEnd();
			_errorsToAdd.Clear();
			foreach (string line in output.Split('\n'))
			{
				if (!line.Contains("error"))
				{
					continue;
				}

				int firstColon = line.IndexOf(':');
				int secondColon = line.IndexOf(':', firstColon + 1);
				int thirdColon = line.IndexOf(':', secondColon + 1);
				int lineNum = Convert.ToInt32(line.Substring(firstColon + 1, secondColon - firstColon - 1));
				string description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
				ArrayList listOfAttributes = new ArrayList { lineNum, description };
				_errorsToAdd.Add(listOfAttributes);
			}
		}

		private void documentParser_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
		{
			foreach (ArrayList attributes in _errorsToAdd)
			{
				AddSquiggleLine((int)attributes[0], Color.Red, attributes[1].ToString());
			}

			if (_documentService.ActiveDocument != null)
			{
				_documentService.ActiveDocument.Refresh();
			}
		}

		private void existingFileMenuItem_Click(object sender, EventArgs e)
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
				_dockingService.ProjectViewer.AddExistingFile(file);
			}
		}

		private void exitMenuItem_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void extractMethodMenuItem_Click(object sender, EventArgs e)
		{
		}

		private void findAllRefsMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			string word = _dockingService.ActiveDocument.GetWord();
			_dockingService.FindResults.NewFindResults(word, _projectService.Project.ProjectName);
			var refs = _projectService.FindAllReferences(word);
			foreach (var fileRef in refs.SelectMany(reference => reference))
			{
				_dockingService.FindResults.AddFindResult(fileRef);
			}

			_dockingService.FindResults.DoneSearching();
			_dockingService.ShowDockPanel(_dockingService.FindResults);
		}

		private void findBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (e.KeyChar != (char)Keys.Enter)
			{
				return;
			}

			if (ActiveMdiChild == null)
			{
				return;
			}

			if (!findBox.Items.Contains(findBox.Text))
			{
				findBox.Items.Add(findBox.Text);
			}

			bool found = _documentService.ActiveDocument.Find(findBox.Text);
			if (!found)
			{
				MessageBox.Show("Text not found");
			}
		}

		private void findInFilesMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				_dockingService.FindForm.ShowFor(this, false, true);
			}
			else
			{
				_dockingService.FindForm.ShowFor(_dockingService.ActiveDocument.EditorBox, false, true);
			}
		}

		private void findMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.FindForm.ShowFor(_dockingService.ActiveDocument.EditorBox, false, false);
		}

		private void formatDocMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.ActiveDocument.FormatLines();
		}

		private void gLabelMenuItem_Click(object sender, EventArgs e)
		{
			GotoSymbol gotoSymbolBox = new GotoSymbol(_parserService);
			if (gotoSymbolBox.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			string symbolString = gotoSymbolBox.inputBox.Text;
			IParserData data = _parserService.GetParserData(symbolString, Settings.Default.caseSensitive).FirstOrDefault();
			if (data == null)
			{
				return;
			}

			IIncludeFile includeFile = data as IIncludeFile;
			if (includeFile != null)
			{
				string fullPath = Path.IsPathRooted(includeFile.IncludedFile) ?
					includeFile.IncludedFile :
					FileOperations.NormalizePath(FindFilePathIncludes(includeFile.IncludedFile));
				_documentService.GotoFile(fullPath);
			}
			else
			{
				_documentService.GotoLabel(data);
			}
		}

		// TODO: remove duplicate code
		private string FindFilePathIncludes(string relativePath)
		{
			IEnumerable<string> includeDirs = _projectService.Project.IsInternal ?
				(IEnumerable<string>)Settings.Default.includeDirs :
				_projectService.Project.IncludeDirs;

			foreach (string dir in includeDirs)
			{
				if (File.Exists(Path.Combine(dir, relativePath)))
				{
					return Path.Combine(dir, relativePath);
				}
			}

			if (_projectService.Project.IsInternal)
			{
				return null;
			}

			return File.Exists(Path.Combine(_projectService.Project.ProjectDirectory, relativePath)) ?
				Path.Combine(_projectService.Project.ProjectDirectory, relativePath) :
				null;
		}

		private void gLineMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			GotoLine gotoBox = new GotoLine(_dockingService.ActiveDocument.TotalNumberOfLines);
			DialogResult gotoResult = gotoBox.ShowDialog();
			if (gotoResult != DialogResult.OK)
			{
				return;
			}

			int line = Convert.ToInt32(gotoBox.inputBox.Text);
			_documentService.GotoLine(_dockingService.ActiveDocument, line);
		}

		private void gotoCurrentToolButton_Click(object sender, EventArgs e)
		{
			_documentService.GotoCurrentDebugLine();
		}

		private void HandleArgs(string[] args)
		{
			if (args.Length == 0)
			{
				return;
			}
			foreach (string arg in args)
			{
				try
				{
					if (string.IsNullOrEmpty(arg))
					{
						break;
					}
					_documentService.OpenDocument(arg);
				}
				catch (FileNotFoundException)
				{
					DockingService.ShowError("Error: File not found");
				}
				catch (Exception ex)
				{
					DockingService.ShowError("Error in loading startup args", ex);
				}
			}
		}

		private void includeDirButton_Click(object sender, EventArgs e)
		{
			IncludeDir includes = new IncludeDir(_projectService.Project);
			includes.ShowDialog();
			includes.Dispose();
		}

		private void invertCaseMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.ActiveDocument.SelectedTextInvertCase();
		}

		private void listFileMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.ActiveDocument.SaveFile();
			string inputFile = _dockingService.ActiveDocument.FileName;
			string outputFile = Path.ChangeExtension(inputFile, "lst");
			string originalDir = Path.GetDirectoryName(inputFile);
			var includeDirs = Settings.Default.includeDirs.Cast<string>();
			_assemblerService.AssembleFile(inputFile, outputFile, originalDir, includeDirs, AssemblyFlags.List | AssemblyFlags.Normal);
		}

		private void LoadStartupProject()
		{
			if (string.IsNullOrEmpty(Settings.Default.startupProject))
			{
				return;
			}

			try
			{
				bool valid = false;
				if (File.Exists(Settings.Default.startupProject))
				{
					valid = OpenProject(Settings.Default.startupProject);
				}
				else
				{
					Settings.Default.startupProject = string.Empty;
					DockingService.ShowError("Error: Project file not found");
				}

				if (_projectService.Project.IsInternal || !valid)
				{
					CreateInternalProject();
				}
			}
			catch (Exception ex)
			{
				CreateInternalProject();
				var result = MessageBox.Show(
								 "There was an error loading the startup project, would you like to remove it?\n" + ex,
								 "Error",
								 MessageBoxButtons.YesNo,
								 MessageBoxIcon.Error);
				if (result == DialogResult.Yes)
				{
					Settings.Default.startupProject = string.Empty;
				}
			}
		}

		private void CreateInternalProject()
		{
			_projectService.CreateInternalProject();
			UpdateProjectMenu(false);
			_dockingService.ProjectViewer.BuildProjTree();
		}

		private bool OpenProject(string fileName)
		{
			bool valid = _projectService.OpenProject(fileName);
			UpdateProjectMenu(true);
			UpdateMenus(_dockingService.Documents.Any());
			_dockingService.ProjectViewer.BuildProjTree();
			return valid;
		}

		private void MainFormRedone_FormClosing(object sender, FormClosingEventArgs e)
		{
			if (_debugger != null)
			{
				CancelDebug();
			}

			if (!_projectService.Project.IsInternal)
			{
				CloseProject();
			}

			try
			{
				SaveWindow();
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error saving window location", ex);
			}

			try
			{
				_dockingService.DestroyService();
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error destroying DockService", ex);
			}

			try
			{
				Settings.Default.Save();
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error saving configuration file", ex);
			}
		}

		private void makeLowerMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.ActiveDocument.SelectedTextToLower();
		}

		private void makeUpperMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.ActiveDocument.SelectedTextToUpper();
		}

		private void newBreakpointMenuItem_Click(object sender, EventArgs e)
		{
			NewBreakpointForm form = new NewBreakpointForm(_dockingService, _documentService);
			form.ShowDialog();
			form.Dispose();
		}

		private void newFileMenuItem_Click(object sender, EventArgs e)
		{
			Editor doc = _documentService.CreateNewDocument();
			_dockingService.ShowDockPanel(doc);
		}

		private void newProjectMenuItem_Click(object sender, EventArgs e)
		{
			NewProjectDialog template = new NewProjectDialog(_dockingService, _documentService, _projectService);
			if (template.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			UpdateProjectMenu(true);
		}

		private void newToolButton_Click(object sender, EventArgs e)
		{
			Editor doc = _documentService.CreateNewDocument();
			doc.TabText = "New Document";
			doc.Show(dockPanel);
		}

		private void nextBookmarkMenuItem_Click(object sender, EventArgs e)
		{
			if (_documentService.ActiveDocument == null)
			{
				return;
			}

			_documentService.ActiveDocument.GotoNextBookmark();
		}

		private void OnAssemblyFinished(object sender, AssemblyFinishEventArgs e)
		{
			_assemblerService.AssemblerProjectFinished -= OnAssemblyFinished;

			_dockingService.MainForm.Invoke(() => ShowErrorPanels(e.Output));
		}

		private void openFileMenuItem_Click(object sender, EventArgs e)
		{
			OpenDocument();
		}

		private void OpenDocument()
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
					string extCheck = Path.GetExtension(fileName);
					if (string.Equals(extCheck, ".wcodeproj", StringComparison.OrdinalIgnoreCase))
					{
						OpenProject(fileName);
						if (Settings.Default.startupProject != fileName)
						{
							if (
								MessageBox.Show("Would you like to make this your default project?",
												"Startup Project",
												MessageBoxButtons.YesNo,
												MessageBoxIcon.Question) == DialogResult.Yes)
							{
								Settings.Default.startupProject = fileName;
							}
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

		private void openProjectMenuItem_Click(object sender, EventArgs e)
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
				if (openFileDialog.ShowDialog() == DialogResult.OK)
				{
					string fileName = openFileDialog.FileName;

					if (!OpenProject(fileName))
					{
						_projectService.CreateInternalProject();
					}

					if (Settings.Default.startupProject != fileName)
					{
						if (
							MessageBox.Show("Would you like to make this your default project?",
											"Startup Project",
											MessageBoxButtons.YesNo,
											MessageBoxIcon.Question) == DialogResult.Yes)
						{
							Settings.Default.startupProject = fileName;
						}
					}
				}
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error opening file.", ex);
			}

			UpdateMenus(_dockingService.Documents.Any());
		}

		/// <summary>
		/// This opens the recend document clicked in the file menu.
		/// </summary>
		/// <param name="sender">This is the button object. This is casted to get which button was clicked.</param>
		/// <param name="e">Nobody cares about this arg.</param>
		private void OpenRecentDoc(object sender, EventArgs e)
		{
			MenuItem button = (MenuItem)sender;
			_documentService.OpenDocument(button.Text);
		}

		private void openToolButton_Click(object sender, EventArgs e)
		{
			OpenDocument();
		}

		private void Paste()
		{
			var activeContent = _dockingService.ActiveContent as ToolWindow;
			if (activeContent != null)
			{
				activeContent.Paste();
			}
			else if (_dockingService.ActiveDocument != null)
			{
				_dockingService.ActiveDocument.Paste();
			}
		}

		private void pasteMenuItem_Click(object sender, EventArgs e)
		{
			Paste();
		}

		private void pasteToolButton_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument != null)
			{
				_dockingService.ActiveDocument.Paste();
			}
		}

		private void pauseToolButton_Click(object sender, EventArgs e)
		{
			_debugger.Pause();
		}

		private void prefsMenuItem_Click(object sender, EventArgs e)
		{
			Preferences prefs = new Preferences(_dockingService);
			prefs.ShowDialog();
		}

		private void prevBookmarkMenuItem_Click(object sender, EventArgs e)
		{
			if (_documentService.ActiveDocument == null)
			{
				return;
			}

			_documentService.ActiveDocument.GotoPrevBookmark();
		}

		private void projStatsMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.ActiveDocument.SaveFile();
			string inputFile = _dockingService.ActiveDocument.FileName;
			string outputFile = string.Empty;
			string originalDir = Path.GetDirectoryName(inputFile);
			var includeDirs = Settings.Default.includeDirs.Cast<string>();
			_assemblerService.AssembleFile(inputFile, outputFile, originalDir, includeDirs, AssemblyFlags.Stats);
		}

		private void redoMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument != null)
			{
				_dockingService.ActiveDocument.Redo();
			}
		}

		private void refreshViewMenuItem_Click(object sender, EventArgs e)
		{
			_dockingService.ProjectViewer.BuildProjTree();
		}

		private void renameMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}
			ShowRefactorForm();
		}

		private void replaceInFilesMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.FindForm.ShowFor(_dockingService.ActiveDocument.EditorBox, true, true);
		}

		private void replaceMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.FindForm.ShowFor(_dockingService.ActiveDocument.EditorBox, true, false);
		}

		private void RestoreWindow()
		{
			try
			{
				WindowState = Settings.Default.WindowState;
				Size = Settings.Default.WindowSize;
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error restoring the window size", ex);
			}
		}

		private void saveAllToolButton_Click(object sender, EventArgs e)
		{
			foreach (Editor child in MdiChildren)
			{
				child.SaveFile();
			}
		}

		private void saveAsMenuItem_Click(object sender, EventArgs e)
		{
			try
			{
				_dockingService.ActiveDocument.SaveFileAs();
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error saving file.", ex);
			}
		}

		private void saveMenuItem_Click(object sender, EventArgs e)
		{
			SaveActiveDocument();
		}

		private void saveProjectMenuItem_Click(object sender, EventArgs e)
		{
			_projectService.SaveProject();
			saveProjectMenuItem.Enabled = _projectService.Project.NeedsSave;
		}

		private void saveToolButton_Click(object sender, EventArgs e)
		{
			SaveActiveDocument();
		}

		private void SaveActiveDocument()
		{
			try
			{
				_dockingService.ActiveDocument.SaveFile();
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error saving file.", ex);
			}
		}

		private void SaveWindow()
		{
			Settings.Default.WindowSize = WindowState != FormWindowState.Normal ? new Size(RestoreBounds.Width, RestoreBounds.Height) : Size;
			Settings.Default.WindowState = WindowState;
		}

		private void selectAllMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_documentService.ActiveDocument.SelectAll();
		}

		private void sentenceCaseMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_documentService.ActiveDocument.SelectedTextToSentenceCase();
		}

		private void ShowErrorPanels(AssemblerOutput output)
		{
			try
			{
				_dockingService.OutputWindow.ClearOutput();
				_dockingService.OutputWindow.AddText(output.OutputText);
				_dockingService.OutputWindow.HighlightOutput();

				// its more fun with colors
				_dockingService.ErrorList.ParseOutput(output.ParsedErrors);
				_dockingService.ShowDockPanel(_dockingService.ErrorList);
				_dockingService.ShowDockPanel(_dockingService.OutputWindow);
				if (_dockingService.ActiveDocument != null)
				{
					_dockingService.ActiveDocument.Refresh();
				}

				foreach (Editor child in _dockingService.Documents)
				{
					child.UpdateIcons(output.ParsedErrors);
				}
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.ToString());
			}
		}

		private void startDebugMenuItem_Click(object sender, EventArgs e)
		{
			if (_debugger == null)
			{
				StartDebug();
			}
			else
			{
				_debugger.Run();
			}
		}

		private void startWithoutDebugMenuItem_Click(object sender, EventArgs e)
		{
			// TODO: fix this
			//_debugger.StartWithoutDebug();
		}

		private void stepButton_Click(object sender, EventArgs e)
		{
			_debugger.Step();
			UpdateDebugStuff();
		}

		private void stepOutMenuItem_Click(object sender, EventArgs e)
		{
			_debugger.StepOut();
			UpdateDebugStuff();
		}

		private void stepOverMenuItem_Click(object sender, EventArgs e)
		{
			_debugger.StepOver();
			UpdateDebugStuff();
		}

		private void symTableMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.ActiveDocument.SaveFile();
			string inputFile = _documentService.ActiveFileName;
			string outputFile = Path.ChangeExtension(_documentService.ActiveFileName, "lab");
			string originalDir = Path.GetDirectoryName(inputFile);
			var includeDirs = Settings.Default.includeDirs.Cast<string>();
			_assemblerService.AssembleFile(inputFile, outputFile, originalDir,
				includeDirs, AssemblyFlags.Normal | AssemblyFlags.Symtable);
		}

		private void toggleBookmarkMenuItem_Click(object sender, EventArgs e)
		{
			if (_documentService.ActiveDocument == null)
			{
				return;
			}

			_documentService.ActiveDocument.ToggleBookmark();
		}

		private void toggleBreakpointMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument == null)
			{
				return;
			}

			_dockingService.ActiveDocument.ToggleBreakpoint();
			_dockingService.ActiveDocument.Refresh();
		}

		private void undoMenuItem_Click(object sender, EventArgs e)
		{
			if (_dockingService.ActiveDocument != null)
			{
				_dockingService.ActiveDocument.Undo();
			}
		}

		private void updateMenuItem_Click(object sender, EventArgs e)
		{
			try
			{
				if (UpdateService.CheckForUpdate())
				{
					var result = MessageBox.Show("New version available. Download now?",
						"Update Available", MessageBoxButtons.YesNo, MessageBoxIcon.None);
					if (result == DialogResult.Yes)
					{
						UpdateService.StartUpdater();
					}
				}
				else
				{
					MessageBox.Show("No new updates");
				}
			}
			catch (Exception ex)
			{
				DockingService.ShowError("Error updating", ex);
			}
		}

		private void UpdateStepOut()
		{
			if (_debugger.StepStack.Count > 0)
			{
				stepOutMenuItem.Enabled = true;
				stepOutToolButton.Enabled = true;
			}
			else
			{
				stepOutMenuItem.Enabled = false;
				stepOutToolButton.Enabled = false;
			}
		}

		/// <summary>
		/// This handles all things relating to the view menu. Just does a switch based
		/// on the tag, and does the appropriate action based on the check mark state
		/// this probably isnt a great way to handle it, but it works
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void viewMenuItem_Click(object sender, EventArgs e)
		{
			ToolStripMenuItem item = (ToolStripMenuItem)sender;
			item.Checked = !item.Checked;
			switch (item.Tag.ToString())
			{
				case "iconBar":
					if (ActiveMdiChild != null)
					{
						_documentService.ActiveDocument.IsIconBarVisible = item.Checked;
					}

					break;
				case "lineNumbers":
					if (ActiveMdiChild != null)
					{
						_documentService.ActiveDocument.ShowLineNumbers = item.Checked;
					}

					break;
				case "labelsList":
					if (item.Checked)
					{
						_dockingService.ShowDockPanel(_dockingService.LabelList);
					}
					else
					{
						_dockingService.HideDockPanel(_dockingService.LabelList);
					}

					break;
				case "mainToolBar":
					if (item.Checked)
					{
						mainToolBar.Show();
					}
					else
					{
						mainToolBar.Hide();
					}

					Settings.Default.mainToolBar = item.Checked;
					break;
				case "editorToolBar":
					if (item.Checked)
					{
						editorToolStrip.Show();
					}
					else
					{
						editorToolStrip.Hide();
					}

					Settings.Default.editorToolbar = item.Checked;
					break;
				case "outputWindow":
					if (item.Checked)
					{
						_dockingService.ShowDockPanel(_dockingService.OutputWindow);
					}
					else
					{
						_dockingService.HideDockPanel(_dockingService.OutputWindow);
					}

					break;
				case "FindResults":
					if (item.Checked)
					{
						_dockingService.ShowDockPanel(_dockingService.FindResults);
					}
					else
					{
						_dockingService.HideDockPanel(_dockingService.FindResults);
					}

					break;
				case "statusBar":
					statusBar.Visible = item.Checked;
					break;
				case "debugPanel":
					if (item.Checked)
					{
						_dockingService.ShowDockPanel(_dockingService.DebugPanel);
					}
					else
					{
						_dockingService.HideDockPanel(_dockingService.DebugPanel);
					}

					break;
				case "callStack":
					if (item.Checked)
					{
						_dockingService.ShowDockPanel(_dockingService.CallStack);
					}
					else
					{
						_dockingService.HideDockPanel(_dockingService.CallStack);
					}

					break;
				case "stackViewer":
					if (item.Checked)
					{
						_dockingService.ShowDockPanel(_dockingService.StackViewer);
					}
					else
					{
						_dockingService.HideDockPanel(_dockingService.StackViewer);
					}

					break;
				case "varTrack":
					if (item.Checked)
					{
						_dockingService.ShowDockPanel(_dockingService.TrackWindow);
					}
					else
					{
						_dockingService.HideDockPanel(_dockingService.TrackWindow);
					}

					break;
				case "breakManager":
					if (item.Checked)
					{
						_dockingService.ShowDockPanel(_dockingService.BreakManagerWindow);
					}
					else
					{
						_dockingService.HideDockPanel(_dockingService.BreakManagerWindow);
					}

					break;
				case "projectViewer":
					if (item.Checked)
					{
						_dockingService.ShowDockPanel(_dockingService.ProjectViewer);
					}
					else
					{
						_dockingService.HideDockPanel(_dockingService.ProjectViewer);
					}

					break;
				case "debugToolBar":
					if (item.Checked)
					{
						debugToolStrip.Show();
					}
					else
					{
						debugToolStrip.Hide();
					}

					Settings.Default.debugToolbar = item.Checked;
					break;
				case "errorList":
					if (item.Checked)
					{
						_dockingService.ShowDockPanel(_dockingService.ErrorList);
					}
					else
					{
						_dockingService.HideDockPanel(_dockingService.ErrorList);
					}

					break;
				case "macroManager":
					if (item.Checked)
					{
						_dockingService.ShowDockPanel(_dockingService.MacroManager);
					}
					else
					{
						_dockingService.HideDockPanel(_dockingService.MacroManager);
					}

					break;
			}

			debugToolStrip.Height = 25;
		}

		internal void UpdateAssembledInfo(string fileName, int lineNumber)
		{
			if (_debugger == null)
			{
				return;
			}
			CalcLocation label = _symbolService.ListTable.GetCalcLocation(fileName, lineNumber);
			if (label == null)
			{
				return;
			}
			string assembledInfo = string.Format("Page: {0} Address: {1}", label.Page, label.Address.ToString("X4"));
			SetToolStripText(assembledInfo);
		}

		internal void AddStackEntry(int lineNumber)
		{
			_debugger.StepStack.Push(lineNumber);
		}

		internal void SetPC(string fileName, int lineNumber)
		{
			_debugger.SetPCToSelect(fileName, lineNumber);

			_documentService.RemoveDebugHighlight();
			_documentService.HighlightDebugLine(lineNumber + 1);
			UpdateDebugPanel();
		}

		internal void ShowRefactorForm()
		{
			RefactorForm form = new RefactorForm(_dockingService, _projectService);
			form.ShowDialog();
		}
	}
}