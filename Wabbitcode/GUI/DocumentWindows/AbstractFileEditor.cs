using System.IO;
using Revsoft.Wabbitcode.Interfaces;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.GUI.DocumentWindows
{
    public abstract class AbstractFileEditor : DockContent, IClipboardOperation, IUndoable, ISelectable
    {
        private string _fileName;
        protected abstract bool DocumentChanged { get; set; }

        /// <summary>
        /// Gets the full path to the file that is open in this editor
        /// </summary>
        public string FileName
        {
            get
            {
                return _fileName;
            }
            set
            {
                Text = value;
                _fileName = value;
                TabText = Path.GetFileName(value);
                ToolTipText = value;
            }
        }

        public virtual void OpenFile(string fileName)
        {
            FileName = fileName;
            DocumentChanged = false;
        }

        internal void SaveFile(string fileName)
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

        public virtual void PersistStringLoad(params string[] values)
        {
            if (string.IsNullOrEmpty(values[1]))
            {
                return;
            }

            string filename = values[1];
            OpenFile(filename);
        }
    }
}