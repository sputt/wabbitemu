using System;
using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace MapEditorPlugin
{
    [UsedImplicitly]
    public partial class TilesPanel : ToolWindow
    {
        private readonly IDockingService _dockingService;

        public TilesPanel()
        {
            InitializeComponent();

            _dockingService = DependencyFactory.Resolve<IDockingService>();
            _dockingService.ActiveDocumentChanged += DockingService_ActiveDocumentChanged;

            DockingService_ActiveDocumentChanged(this, EventArgs.Empty);
        }

        private void DockingService_ActiveDocumentChanged(object sender, EventArgs eventArgs)
        {
            var document = _dockingService.ActiveDocument as MapFileEditor;
            tilesPanel.DataContext = document != null ? document.AppModel : null;
        }
    }
}
