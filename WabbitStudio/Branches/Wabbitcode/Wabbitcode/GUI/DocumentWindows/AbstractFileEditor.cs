using System;
using System.Collections.Specialized;
using System.IO;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Interfaces;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Utils;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.GUI.DocumentWindows
{
    public abstract class AbstractFileEditor : DockContent, IClipboardOperation, IUndoable, ISelectable, IDisposable
    {
        #region Readonly Variables

        private readonly IProjectService _projectService = DependencyFactory.Resolve<IProjectService>();

        #endregion

        #region Private Fields

        private FilePath _fileName;
        private volatile bool _wasExternallyModified;
        private volatile bool _needsReloaded;
        private bool _hasEntered;

        #endregion

        #region Properties

        protected virtual bool DocumentChanged { get; set; }

        /// <summary>
        /// Gets the full path to the file that is open in this editor
        /// </summary>
        public FilePath FileName
        {
            get { return _fileName; }
            set
            {
                Text = value;
                _fileName = value;
                TabText = Path.GetFileName(value);
                ToolTipText = value;
            }
        }

        #endregion

        protected AbstractFileEditor()
        {
            if (_projectService.Project != null)
            {
                _projectService.Project.FileModifiedExternally += Project_FileModifiedExternally;
            }

            Enter += AbstractFileEditor_Enter;
            Leave += AbstractFileEditor_Leave;
            _projectService.ProjectOpened += ProjectService_OnProjectOpened;
            _projectService.ProjectClosed += ProjectService_OnProjectClosed;
        }

        ~AbstractFileEditor()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
        }

        protected virtual void Dispose(bool isDisposing)
        {
            if (isDisposing)
            {
                Enter -= AbstractFileEditor_Enter;
                Leave -= AbstractFileEditor_Leave;

                if (_projectService.Project != null)
                {
                    _projectService.Project.FileModifiedExternally -= Project_FileModifiedExternally;
                }
            }
        }

        public virtual void OpenFile(FilePath fileName)
        {
            FileName = fileName;
            DocumentChanged = false;

            if (Settings.Default.RecentFiles == null)
            {
                Settings.Default.RecentFiles = new StringCollection();
            }

            if (!Settings.Default.RecentFiles.Contains(fileName))
            {
                Settings.Default.RecentFiles.Add(fileName);
            }
        }

        public virtual void SaveFile(FilePath fileName)
        {
            if (string.IsNullOrEmpty(fileName))
            {
                return;
            }

            FileName = fileName;
            SaveFile();
        }

        public void SaveFile()
        {
            _projectService.Project.EnableFileWatcher(false);
            TabText = Path.GetFileName(FileName);
            DocumentChanged = false;
            SaveFileInner();
            _projectService.Project.EnableFileWatcher(true);
        }

        protected abstract void SaveFileInner();

        protected virtual void CloseFile()
        {
            _projectService.ProjectOpened -= ProjectService_OnProjectOpened;
            _projectService.ProjectClosed -= ProjectService_OnProjectClosed;

            if (_projectService.Project != null)
            {
                _projectService.Project.FileModifiedExternally -= Project_FileModifiedExternally;
            }
        }

        protected override string GetPersistString()
        {
            // Add extra information into the persist string for this document
            // so that it is available when deserialized.
            return GetType().AssemblyQualifiedName + ";" + FileName;
        }

        public abstract void Copy();
        public abstract void Cut();
        public abstract void Paste();
        public abstract void Undo();
        public abstract void Redo();
        public abstract void SelectAll();
        protected abstract void ReloadFile();

        public virtual void PersistStringLoad(params string[] values)
        {
            // Nothing to do by default
        }


        private void ProjectService_OnProjectClosed(object sender, EventArgs eventArgs)
        {
            _projectService.Project.FileModifiedExternally -= Project_FileModifiedExternally;
        }

        private void ProjectService_OnProjectOpened(object sender, EventArgs eventArgs)
        {
            _projectService.Project.FileModifiedExternally += Project_FileModifiedExternally;
        }

        private void Project_FileModifiedExternally(object sender, FileModifiedEventArgs e)
        {
            if (e.File.FileFullPath != FileName || _wasExternallyModified)
            {
                return;
            }

            if (InvokeRequired)
            {
                this.BeginInvoke(() => Project_FileModifiedExternally(sender, e));
                return;
            }

            var project = _projectService.Project;
            if (project != null && project.BuildSystem.IsBuilding)
            {
                return;
            }

            _needsReloaded = true;
            if (!_hasEntered)
            {
                return;
            }

            PromptReloadFile();
        }

        private void AbstractFileEditor_Enter(object sender, EventArgs e)
        {
            _hasEntered = true;
            if (_wasExternallyModified || !_needsReloaded)
            {
                return;
            }

            PromptReloadFile();
        }

        private void AbstractFileEditor_Leave(object sender, EventArgs e)
        {
            _hasEntered = false;
        }

        private void PromptReloadFile()
        {
            _needsReloaded = false;
            _wasExternallyModified = true;
            const string modifiedFormat = "{0} modified outside the editor.\nLoad changes?";
            DialogResult result = MessageBox.Show(this, string.Format(modifiedFormat, FileName),
                "File modified", MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes)
            {
                ReloadFile();
                DocumentChanged = false;
            }
            else
            {
                DocumentChanged = true;
            }
            _wasExternallyModified = false;
        }
    }
}