// Copyright (c) AlphaSierraPapa for the SharpDevelop Team (for details please see \doc\copyright.txt)
// This code is distributed under the GNU LGPL (for details please see \doc\license.txt)

using System;
using System.Linq;
using System.Text;
using System.Windows;
using System.Collections.ObjectModel;
using System.Collections.Generic;

//using Revsoft.SharpDevelop;

namespace Revsoft.Wabbitcode.AvalonEditExtension
{
	/// <summary>
	/// Interaction logic for ChooseEncodingDialog.xaml
	/// </summary>
	public partial class ChooseEncodingDialog : Window
	{
        static readonly IEnumerable<EncodingInfo> allEncodings = Encoding.GetEncodings().OrderBy(e => e.DisplayName).ToList();
        static int defaultFileEncodingCodePage = 65001;
        static EncodingInfo DefaultFileEncoding
        {
            get
            {
                int cp = defaultFileEncodingCodePage;
                return allEncodings.Single(e => e.CodePage == cp);
            }
            set
            {
                defaultFileEncodingCodePage = value.CodePage;
            }
        }

		public ChooseEncodingDialog()
		{
			InitializeComponent();
			encodingComboBox.ItemsSource = allEncodings;
			encodingComboBox.SelectedItem = DefaultFileEncoding;
		}
		
		public Encoding Encoding {
			get { return ((EncodingInfo)encodingComboBox.SelectedItem).GetEncoding(); }
			set { encodingComboBox.SelectedItem = allEncodings.Single(e => e.CodePage == value.CodePage); }
		}
		
		void okButton_Click(object sender, RoutedEventArgs e)
		{
			DialogResult = true;
			Close();
		}
	}
}
