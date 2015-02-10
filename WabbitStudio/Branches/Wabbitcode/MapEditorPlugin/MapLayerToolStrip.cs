using System;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;
using WPFZ80MapEditor;

namespace MapEditorPlugin
{
    public sealed class MapLayerToolStrip : ToolStrip
    {
        public const string StripName = "Map Layer Toolbar";

        private readonly IDockingService _dockingService = DependencyFactory.Resolve<IDockingService>();

        private readonly ToolStripButton _mapsetStripButton = new ToolStripButton
        {
            Name = "Mapset",
            Text = "Mapset",
            Tag = LayerType.MapsetLayer,
            DisplayStyle = ToolStripItemDisplayStyle.Text,
            CheckOnClick = true
        };

        private readonly ToolStripButton _tilegroupStripButton = new ToolStripButton
        {
            Name = "Tilegroup",
            Text = "Tilegroup",
            Tag = LayerType.TilegroupLayer,
            DisplayStyle = ToolStripItemDisplayStyle.Text,
            CheckOnClick = true
        };

        private readonly ToolStripButton _mapStripButton = new ToolStripButton
        {
            Name = "Map",
            Text = "Map",
            Tag = LayerType.MapLayer,
            DisplayStyle = ToolStripItemDisplayStyle.Text,
            CheckOnClick = true
        };

        private readonly ToolStripButton _objectsStripButton = new ToolStripButton
        {
            Name = "Objects",
            Text = "Objects",
            Tag = LayerType.ObjectLayer,
            DisplayStyle = ToolStripItemDisplayStyle.Text,
            CheckOnClick = true
        };

        private readonly ToolStripButton _enemiesStripButton = new ToolStripButton
        {
            Name = "Enemies",
            Text = "Enemies",
            Tag = LayerType.EnemyLayer,
            DisplayStyle = ToolStripItemDisplayStyle.Text,
            CheckOnClick = true
        };

        private readonly ToolStripButton _miscStripButton = new ToolStripButton
        {
            Name = "Misc",
            Text = "Misc",
            Tag = LayerType.MiscLayer,
            DisplayStyle = ToolStripItemDisplayStyle.Text,
            CheckOnClick = true
        };

        private bool _isUpdating;

        public MapLayerToolStrip()
        {
            Items.AddRange(new ToolStripItem[]
            {
                _mapsetStripButton,
                _tilegroupStripButton,
                _mapStripButton,
                _objectsStripButton,
                _enemiesStripButton,
                _miscStripButton
            });

            _mapsetStripButton.CheckedChanged += StripButton_CheckedChanged;
            _tilegroupStripButton.CheckedChanged += StripButton_CheckedChanged;
            _mapStripButton.CheckedChanged += StripButton_CheckedChanged;
            _objectsStripButton.CheckedChanged += StripButton_CheckedChanged;
            _enemiesStripButton.CheckedChanged += StripButton_CheckedChanged;
            _miscStripButton.CheckedChanged += StripButton_CheckedChanged;

            Text = StripName;
        }

        private void StripButton_CheckedChanged(object sender, EventArgs e)
        {
            var layerType = (LayerType) ((ToolStripButton) sender).Tag;
            UpdateChecks(layerType);
        }

        public void UpdateChecks(LayerType layerType)
        {
            if (_isUpdating)
            {
                return;
            }

            _isUpdating = true;

            foreach (ToolStripButton item in Items)
            {
                item.Checked = false;
            }

            var document = _dockingService.ActiveDocument as MapFileEditor;
            if (document == null)
            {
                return;
            }

            document.AppModel.CurrentLayer = layerType;
            switch (document.AppModel.CurrentLayer)
            {
                case LayerType.MapsetLayer:
                    ((ToolStripButton)Items["Mapset"]).Checked = true;
                    break;
                case LayerType.TilegroupLayer:
                    ((ToolStripButton)Items["Tilegroup"]).Checked = true;
                    break;
                case LayerType.MapLayer:
                    ((ToolStripButton)Items["Map"]).Checked = true;
                    break;
                case LayerType.ObjectLayer:
                    ((ToolStripButton)Items["Objects"]).Checked = true;
                    break;
                case LayerType.EnemyLayer:
                    ((ToolStripButton)Items["Enemies"]).Checked = true;
                    break;
                case LayerType.MiscLayer:
                    ((ToolStripButton)Items["Misc"]).Checked = true;
                    break;
                case LayerType.TestingLayer:
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            _isUpdating = false;
        }
    }
}