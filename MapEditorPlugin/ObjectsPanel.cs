using System;
using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace MapEditorPlugin
{
    [UsedImplicitly]
    public partial class ObjectsPanel : ToolWindow
    {
        private readonly IDockingService _dockingService;

        public ObjectsPanel()
        {
            InitializeComponent();

            _dockingService = DependencyFactory.Resolve<IDockingService>();
            _dockingService.ActiveDocumentChanged += DockingService_ActiveDocumentChanged;

            DockingService_ActiveDocumentChanged(this, EventArgs.Empty);
        }

        private void DockingService_ActiveDocumentChanged(object sender, EventArgs eventArgs)
        {
            var document = _dockingService.ActiveDocument as MapFileEditor;
            if (document != null)
            {
                objectPanel.DataContext = document.AppModel;
                WPFZ80MapEditor.ObjectsPanel.DragScope = document.Child;
            }
            else
            {
                objectPanel.DataContext = null;
            }
        }
    }
}
