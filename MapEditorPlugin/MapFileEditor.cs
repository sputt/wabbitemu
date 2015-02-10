using System.IO;
using System.Linq;
using System.Windows;
using Revsoft.Wabbitcode.GUI.DocumentWindows;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;
using WPFZ80MapEditor;

namespace MapEditorPlugin
{
    public partial class MapFileEditor : AbstractFileEditor
    {
        protected override bool DocumentChanged
        {
            get { return base.DocumentChanged || UndoManager.CanRedo(); }
            set { base.DocumentChanged = value; }
        }

        public static bool OpenDocument(FilePath filename)
        {
            var dockingService = DependencyFactory.Resolve<IDockingService>();
            var child = dockingService.Documents.OfType<MapFileEditor>().SingleOrDefault(e => e.FileName == filename);
            if (child != null)
            {
                child.Show();
                return true;
            }

            var name = Path.GetFileName(filename);
            var doc = new MapFileEditor()
            {
                Text = name,
                TabText = name,
                ToolTipText = filename
            };

            doc.OpenFile(filename);


            dockingService.ShowDockPanel(doc);
            return true;
        }

        private MapFileEditor()
        {
            InitializeComponent();
        }

        public override void Copy()
        {
            
        }

        public override void Cut()
        {
            
        }

        public override void Paste()
        {
            
        }

        public override void Undo()
        {
            UndoManager.Undo(_editor.Model);
        }

        public override void Redo()
        {
            UndoManager.Redo(_editor.Model);
        }

        public override void SelectAll()
        {
            
        }

        protected override void ReloadFile()
        {
            OpenFile(FileName);
        }

        public override void OpenFile(FilePath fileName)
        {
            base.OpenFile(fileName);
            _editor.OpenScenario(fileName);
        }

        protected override void SaveFileInner()
        {
            _editor.SaveScenario(FileName);
        }

        public AppModel AppModel
        {
            get { return _editor.Model; }
        }

        public FrameworkElement Child
        {
            get { return _editor; }
        }
    }
}
