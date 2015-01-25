using System;
using System.Collections.Generic;
using System.Windows.Forms;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    public partial class StructureDefiner : Form
    {
        private readonly List<TreeStructureModel> _currentStructMembers = new List<TreeStructureModel>();
        private readonly IProjectService _projectService;

        public StructureDefiner()
        {
            InitializeComponent();

            _projectService = DependencyFactory.Resolve<IProjectService>();
        }

        private void sturctureGrid_RowsAdded(object sender, DataGridViewRowsAddedEventArgs e)
        {
            if (e.RowIndex > _currentStructMembers.Count)
            {
                _currentStructMembers.Add(ConvertRow(sturctureGrid.Rows[e.RowIndex]));
            }
        }

        private void sturctureGrid_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            _currentStructMembers[e.RowIndex] = ConvertRow(sturctureGrid.Rows[e.RowIndex]);
        }

        private static TreeStructureModel ConvertRow(DataGridViewRow dataGridViewRow)
        {
            var cells = dataGridViewRow.Cells;
            string cellName = (string) (cells[0].Value ?? string.Empty);
            int value = cells[1] != null ? Convert.ToInt32(cells[1].Value) : 0;
            string enumValue = cells[2] != null && cells[2].Value != null ? 
                cells[2].Value.ToString() : 
                VariableDisplayMethod.Hexadecimal.ToString();
            VariableDisplayMethod method = (VariableDisplayMethod) Enum.Parse(typeof (VariableDisplayMethod), enumValue);
            return new TreeStructureModel
            {
                Name = cellName,
                Size = value,
                Type = method
            };
        }
    }
}