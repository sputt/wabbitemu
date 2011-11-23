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

namespace Revsoft.Wabbitcode.Panels
{
	/// <summary>
	/// Interaction logic for LabelList.xaml
	/// </summary>
	public partial class OutputWindow : DockableContent, IWabbitcodePanel
	{
		public OutputWindow()
		{
			InitializeComponent();
		}

		public void Cut()
		{
			throw new InvalidOperationException();
		}

		public void Copy()
		{
			OutputBox.Copy();
		}

		public void Paste()
		{
			throw new InvalidOperationException();
		}

		public void Undo()
		{
			throw new InvalidOperationException();
		}

		public void Redo()
		{
			throw new InvalidOperationException();
		}

		internal void ClearText()
		{
			OutputBox.Clear();
		}

		internal new void AddText(string text)
		{
			OutputBox.Text += text;
		}
	}
}
