using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using AvalonDock;
using Revsoft.Wabbitcode.Interface;
using System.ComponentModel;

namespace Revsoft.Wabbitcode.Panels
{
	/// <summary>
	/// Interaction logic for LabelList.xaml
	/// </summary>
	public partial class LabelList : DockableContent, IWabbitcodePanel
	{
		public LabelList()
		{
			InitializeComponent();
		}

		#region IClipboard
		public void Cut()
		{
			throw new NotImplementedException();
		}

		public void Copy()
		{
			throw new NotImplementedException();
		}

		public void Paste()
		{
			throw new NotImplementedException();
		}
		#endregion

		#region IUndoable
		public void Undo()
		{
			throw new NotImplementedException();
		}

		public void Redo()
		{
			throw new NotImplementedException();
		}
		#endregion
	}
}
