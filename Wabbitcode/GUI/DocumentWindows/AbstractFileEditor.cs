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
    public abstract class AbstractFileEditor : DockContent, IClipboardOperation, IUndoable, ISelectable
    {
        #region Readonly Variables

        private readonly IProjectService _projectService = DependencyFactory.Resolve<IProjectService>();

        #endregion

        #region Private Fields

        private FilePath _fileName;
        private volatile bool _wasExternallyModified;

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

            _projectService.ProjectOpened += ProjectService_OnProjectOpened;
            _projectService.ProjectClosed += ProjectService_OnProjectClosed;
        }

        protected virtual void OpenFile(FilePath fileName)
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

        public void SaveFile(FilePath fileName)
        {
            if (string.IsNullOrEmpty(fileName))
            {
                return;
            }

            FileName = fileName;
            SaveFile();
        }

        public virtual void SaveFile()
        {
            TabText = Path.GetFileName(FileName);
            DocumentChanged = false;
        }

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
            return GetType() + ";" + FileName;
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
            if (values == null || values.Length < 2)
            {
                return;
            }

            if (string.IsNullOrEmpty(values[1]))
            {
                return;
            }

            string filename = values[1];
            OpenFile(new FilePath(filename));
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

            if (IsDisposed)
            {
                return;
            }

            if (InvokeRequired)
            {
                this.BeginInvoke(() => Project_FileModifiedExternally(sender, e));
                return;
            }

            _wasExternallyModified = true;
            const string modifiedFormat = "{0} modified outside the editor.\nLoad changes?";
            DialogResult result = MessageBox.Show(this, string.Format(modifiedFormat, e.File.FileFullPath),
                "File modified", MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes)
            {
                ReloadFile();
            }
            else
            {
                DocumentChanged = true;
            }
            _wasExternallyModified = false;
        }
    }
}