using System;
using System.Drawing;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Windows.Forms;

namespace Wabbitcode
{
	/// <summary>
	/// Summary description for AutoCompleteForm.
	/// </summary>
	public class AutoCompleteForm : System.Windows.Forms.Form
	{
		private StringCollection mItems = new StringCollection();
		private System.Windows.Forms.ListView lstCompleteItems;
		private System.Windows.Forms.ColumnHeader columnHeader1;

		public StringCollection Items 
		{
			get 
			{
				return mItems;
			}
		}

		internal int ItemHeight 
		{
			get  
			{
				return 18;
			}
		}

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public AutoCompleteForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
		}

		public string SelectedItem 
		{
			get
			{
				if (lstCompleteItems.SelectedItems.Count == 0) return null;
				return (string)lstCompleteItems.SelectedItems[0].Text;
			}
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.lstCompleteItems = new System.Windows.Forms.ListView();
			this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
			this.SuspendLayout();
			// 
			// lstCompleteItems
			// 
			this.lstCompleteItems.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
																							   this.columnHeader1});
			this.lstCompleteItems.Dock = System.Windows.Forms.DockStyle.Fill;
			this.lstCompleteItems.FullRowSelect = true;
			this.lstCompleteItems.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
			this.lstCompleteItems.HideSelection = false;
			this.lstCompleteItems.LabelWrap = false;
			this.lstCompleteItems.Location = new System.Drawing.Point(0, 0);
			this.lstCompleteItems.MultiSelect = false;
			this.lstCompleteItems.Name = "lstCompleteItems";
			this.lstCompleteItems.Size = new System.Drawing.Size(152, 136);
			this.lstCompleteItems.Sorting = System.Windows.Forms.SortOrder.Ascending;
			this.lstCompleteItems.TabIndex = 1;
			this.lstCompleteItems.View = System.Windows.Forms.View.Details;
			// 
			// columnHeader1
			// 
			this.columnHeader1.Width = 148;
			// 
			// AutoCompleteForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(152, 136);
			this.ControlBox = false;
			this.Controls.Add(this.lstCompleteItems);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.MaximizeBox = false;
			this.MaximumSize = new System.Drawing.Size(128, 176);
			this.MinimizeBox = false;
			this.Name = "AutoCompleteForm";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "AutoCompleteForm";
			this.TopMost = true;
			this.Resize += new System.EventHandler(this.AutoCompleteForm_Resize);
			this.VisibleChanged += new System.EventHandler(this.AutoCompleteForm_VisibleChanged);
			this.ResumeLayout(false);

		}
		#endregion

		private void lstCompleteItems_SelectedIndexChanged(object sender, System.EventArgs e)
		{
		}

		internal int SelectedIndex 
		{
			get 
			{
				if (lstCompleteItems.SelectedIndices.Count == 0)
				{
					return -1;
				}
				return lstCompleteItems.SelectedIndices[0];
			}
			set
			{
				lstCompleteItems.Items[value].Selected = true;
			}
		}
		private void AutoCompleteForm_Resize(object sender, System.EventArgs e)
		{
			//System.Diagnostics.Debug.WriteLine(string.Format("Size x:{0} y:{1}\r\n {2}", Size.Width , Size.Height, Environment.StackTrace));
		}

		internal void UpdateView()
		{
			lstCompleteItems.Items.Clear();
			foreach (string item in mItems)
			{
				lstCompleteItems.Items.Add(item);
			}
		}

		private void AutoCompleteForm_VisibleChanged(object sender, System.EventArgs e)
		{
			ArrayList items = new ArrayList(mItems);
			items.Sort(new CaseInsensitiveComparer());
			mItems = new StringCollection();
			mItems.AddRange((string[])items.ToArray(typeof(string)));
			columnHeader1.Width = lstCompleteItems.Width - 20;

		}

		private void lstCompleteItems_Resize(object sender, System.EventArgs e)
		{
			if (this.Size != lstCompleteItems.Size)
			{
				
			}
		}
	}
}
