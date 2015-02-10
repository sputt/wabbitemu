using System;
using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;
using WPFZ80MapEditor;

namespace MapEditorPlugin
{
    [UsedImplicitly]
    public class MapEditorPlugin : IWabbitcodePlugin
    {
        private IDockingService _dockingService;
        private IProjectService _projectService;
        private IToolBarService _toolbarService;
        private MapLayerToolStrip _toolStrip;

        public string Name
        {
            get { return "MapEditor"; }
        }

        public void Loaded()
        {
            _dockingService = DependencyFactory.Resolve<IDockingService>();
            _projectService = DependencyFactory.Resolve<IProjectService>();
            _toolbarService = DependencyFactory.Resolve<IToolBarService>();

            _toolStrip = new MapLayerToolStrip();
            _toolbarService.RegisterToolbar(MapLayerToolStrip.StripName, _toolStrip);

            _projectService.ProjectOpened += ProjectService_ProjectOpened;


            if (_projectService.Project != null)
            {
                Initialize();
            }
        }

        private void ProjectService_ProjectOpened(object sender, EventArgs e)
        {
            if (_projectService.Project.IsInternal)
            {
                return;
            }

            Initialize();
        }

        private void Initialize()
        {
            MapEditorControl.ZeldaFolder = _projectService.Project.ProjectDirectory;
            var fileTypeFactory = DependencyFactory.Resolve<IFileTypeMethodFactory>();
            fileTypeFactory.RegisterFileType(".zmap", MapFileEditor.OpenDocument);
            _dockingService.ActiveDocumentChanged += DockingService_ActiveDocumentChanged;
        }

        private void DockingService_ActiveDocumentChanged(object sender, EventArgs eventArgs)
        {
            var document = _dockingService.ActiveDocument as MapFileEditor;
            if (document == null)
            {
                _dockingService.HideDockPanel<TilesPanel>();
                _dockingService.HideDockPanel<ObjectsPanel>();
                _toolbarService.HideToolBar(MapLayerToolStrip.StripName);
            }
            else
            {
                _dockingService.ShowDockPanel<TilesPanel>();
                _dockingService.ShowDockPanel<ObjectsPanel>();
                _toolbarService.ShowToolBar(MapLayerToolStrip.StripName);

                document.AppModel.PropertyChanged += AppModel_PropertyChanged;
                _toolStrip.UpdateChecks(document.AppModel.CurrentLayer);
            }
        }

        private void AppModel_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "CurrentLayer")
            {
                _toolStrip.UpdateChecks(((AppModel)sender).CurrentLayer);
            }
        }

        public void Unloaded()
        {
            _projectService.ProjectOpened -= ProjectService_ProjectOpened;
        }
    }
}
