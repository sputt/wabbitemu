using System;
using System.Collections.Generic;
using System.Windows.Forms;
using Revsoft.Wabbitcode.GUI.Dialogs;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class TrackingWindow
    {
        private System.Windows.Forms.MenuItem autoHideItem;

        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.ContextMenu contextMenu;
        private System.Windows.Forms.MenuItem floatingItem;
        private System.Windows.Forms.MenuItem hideItem;
        private Aga.Controls.Tree.TreeColumn variableAddressCol;
        private Aga.Controls.Tree.TreeColumn numBytesCol;
        private Aga.Controls.Tree.TreeColumn valueTypeCol;
        private Aga.Controls.Tree.TreeColumn variableValueCol;
        private Aga.Controls.Tree.TreeViewAdv variablesDataView;
        private Aga.Controls.Tree.NodeControls.NodeTextBox _nameBox;
        private Aga.Controls.Tree.NodeControls.NodeTextBox _addressBox;
        private Aga.Controls.Tree.NodeControls.NodeTextBox _numBytesBox;
        private Aga.Controls.Tree.NodeControls.NodeComboBox _valueTypeBox;
        private Aga.Controls.Tree.NodeControls.NodeTextBox _valueBox;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (this.components != null))
            {
                this.components.Dispose();
                foreach (BitmapViewer viewer in _openBitmaps)
                {
                    viewer.Dispose();
                }
            }

            base.Dispose(disposing);
        }

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TrackingWindow));
            this.contextMenu = new System.Windows.Forms.ContextMenu();
            this.autoHideItem = new System.Windows.Forms.MenuItem();
            this.floatingItem = new System.Windows.Forms.MenuItem();
            this.hideItem = new System.Windows.Forms.MenuItem();
            this.variablesDataView = new Aga.Controls.Tree.TreeViewAdv();
            this.variableAddressCol = new Aga.Controls.Tree.TreeColumn();
            this.numBytesCol = new Aga.Controls.Tree.TreeColumn();
            this.valueTypeCol = new Aga.Controls.Tree.TreeColumn();
            this.variableValueCol = new Aga.Controls.Tree.TreeColumn();
            this._nameBox = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this._addressBox = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this._numBytesBox = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this._valueTypeBox = new Aga.Controls.Tree.NodeControls.NodeComboBox();
            this._valueBox = new Aga.Controls.Tree.NodeControls.NodeTextBox();
            this.SuspendLayout();
            // 
            // autoHideItem
            // 
            this.autoHideItem.Index = -1;
            this.autoHideItem.Text = "Autohide";
            // 
            // floatingItem
            // 
            this.floatingItem.Index = -1;
            this.floatingItem.Text = "Floating";
            // 
            // hideItem
            // 
            this.hideItem.Index = -1;
            this.hideItem.Text = "Hide";
            // 
            // variablesDataView
            // 

            this.variablesDataView.AllowColumnReorder = true;
            this.variablesDataView.Columns.Add(this.variableAddressCol);
            this.variablesDataView.Columns.Add(this.numBytesCol);
            this.variablesDataView.Columns.Add(this.valueTypeCol);
            this.variablesDataView.Columns.Add(this.variableValueCol);
            this.variablesDataView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.variablesDataView.GridLineStyle = ((Aga.Controls.Tree.GridLineStyle)((Aga.Controls.Tree.GridLineStyle.Horizontal | Aga.Controls.Tree.GridLineStyle.Vertical)));
            this.variablesDataView.Model = null;
            this.variablesDataView.Name = "variablesDataView";
            this.variablesDataView.NodeControls.Add(this._nameBox);
            this.variablesDataView.NodeControls.Add(this._addressBox);
            this.variablesDataView.NodeControls.Add(this._numBytesBox);
            this.variablesDataView.NodeControls.Add(this._valueTypeBox);
            this.variablesDataView.NodeControls.Add(this._valueBox);
            this.variablesDataView.LineColor = System.Drawing.SystemColors.ControlDark;
            this.variablesDataView.Location = new System.Drawing.Point(0, 0);
            this.variablesDataView.SelectedNode = null;
            this.variablesDataView.SelectionMode = Aga.Controls.Tree.TreeSelectionMode.Single;
            this.variablesDataView.Size = new System.Drawing.Size(1107, 266);
            this.variablesDataView.TabIndex = 0;
            this.variablesDataView.UseColumns = true;
            this.variablesDataView.MouseDoubleClick += variablesDataView_MouseDoubleClick;
            this.variablesDataView.KeyDown += variablesDataView_KeyDown;

            // 
            // variableAddressCol
            // 
            this.variableAddressCol.Header = "Variable name";
            this.variableAddressCol.Width = 300;
            this.variableAddressCol.SortOrder = SortOrder.None;
            // 
            // numBytesCol
            // 
            this.numBytesCol.Header = "Bytes";
            this.numBytesCol.Width = 80;
            this.numBytesCol.SortOrder = SortOrder.None;
            // 
            // valueTypeCol
            // 
            this.valueTypeCol.Header = "Type";
            this.valueTypeCol.Width = 150;
            this.valueTypeCol.SortOrder = SortOrder.None;
            // 
            // variableValueCol
            // 
            this.variableValueCol.Header = "Value";
            this.variableValueCol.SortOrder = SortOrder.None;
            this.variableValueCol.Width = 400;
            //
            // _addressBox
            //
            this._addressBox.DataPropertyName = "Address";
            this._addressBox.EditEnabled = true;
            this._addressBox.Trimming = System.Drawing.StringTrimming.EllipsisCharacter;
            this._addressBox.UseCompatibleTextRendering = true;
            this._addressBox.ParentColumn = this.variableAddressCol;
            this._addressBox.ChangesApplied += AddressBoxOnChangesApplied;
            //
            // _numBytesBox
            //
            this._numBytesBox.DataPropertyName = "NumBytesString";
            this._numBytesBox.EditEnabled = true;
            this._numBytesBox.Trimming = System.Drawing.StringTrimming.EllipsisCharacter;
            this._numBytesBox.UseCompatibleTextRendering = true;
            this._numBytesBox.ParentColumn = this.numBytesCol;
            this._numBytesBox.ChangesApplied += NumBytesBox_OnChangesApplied;

            //
            // _valueTypeBox
            //
            this._valueTypeBox.DataPropertyName = "ValueType";
            this._valueTypeBox.EditEnabled = true;
            this._valueTypeBox.ChangesApplied += ValueTypeBox_OnChangesApplied;
            this._valueTypeBox.ParentColumn = this.valueTypeCol;

            //
            // _valueBox
            //
            this._valueBox.DataPropertyName = "Value";
            this._valueBox.EditEnabled = false;
            this._valueBox.ParentColumn = this.variableValueCol;

            // 
            // TrackingWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1107, 266);
            this.Controls.Add(this.variablesDataView);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "TrackingWindow";
            this.ShowHint = WeifenLuo.WinFormsUI.Docking.DockState.DockBottom;
            this.TabPageContextMenu = this.contextMenu;
            this.TabText = "Watches";
            this.Text = "Watches";
            this.ResumeLayout(false);
        }
    }
}