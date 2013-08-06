using System.Linq;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class TrackingWindow : ToolWindow
	{
		private readonly List<BitmapViewer> _openBitmaps = new List<BitmapViewer>();
		private readonly List<DebugVariable> _variables = new List<DebugVariable>();
		private WabbitcodeDebugger _debugger;
		private readonly ISymbolService _symbolService;

		public TrackingWindow(IDockingService dockingService, ISymbolService symbolService)
			: base(dockingService)
		{
			InitializeComponent();

			dockingService.MainForm.OnDebuggingStarted += mainForm_OnDebuggingStarted;
			_symbolService = symbolService;
		}

		void mainForm_OnDebuggingStarted(object sender, Services.Debugger.DebuggingEventArgs e)
		{
			_debugger = e.Debugger;
		}

		public override void Copy()
		{
			if (variablesDataView != null)
			{
				Clipboard.SetDataObject(variablesDataView.GetClipboardContent());
			}
		}

		public override void Paste()
		{
			if (variablesDataView.EditingControl != null)
			{
				variablesDataView.EditingControl.Text = Clipboard.GetData(DataFormats.Text).ToString();
			}
		}

		public void UpdateVars()
		{
			for (int i = 0; i < _variables.Count; i++)
			{
				DebugVariable var = _variables[i];
				if (var.RowNumber > variablesDataView.Rows.Count)
				{
					continue;
				}

				// if (variablesDataView.Rows[var.rowNumber].Cells[0].Value != null)
				//    var.name = variablesDataView.SelectedRows[0].Cells[0].Value.ToString();
				if (var.Value.ToString() == "Error")
				{
					continue;
				}
				var = GetValue(var);
				var value = var.Value as Bitmap;
				if (value != null)
				{
					variablesDataView.Rows[var.RowNumber].Cells[5].Value = "Double click for screen";
					foreach (BitmapViewer test in _openBitmaps)
					{
						if (test.Tag.ToString() == var.RowNumber.ToString())
						{
							test.pictureBox.Image = (Image)var.Value;
						}
					}
				}
				else
				{
					variablesDataView.Rows[var.RowNumber].Cells[5].Value = var.Value;
				}
				_variables[i] = var;
			}
		}

		private DebugVariable GetValue(DebugVariable vartoadd)
		{
			if (vartoadd.RowNumber > variablesDataView.Rows.Count)
			{
				return vartoadd;
			}
			if (variablesDataView.Rows[vartoadd.RowNumber].Cells[3].Value == null)
			{
				variablesDataView.Rows[vartoadd.RowNumber].Cells[3].Value = "Byte";
			}
			string valueTypeEntry = variablesDataView.Rows[vartoadd.RowNumber].Cells[3].Value.ToString();
			switch (valueTypeEntry)
			{
				case "Image":
					bool isGrayscale = false;
					if (variablesDataView.Rows[vartoadd.RowNumber].Cells[4].Value == null)
					{
						variablesDataView.Rows[vartoadd.RowNumber].Cells[4].Value = 0;
					}
					if (variablesDataView.Rows[vartoadd.RowNumber].Cells[4].Value.ToString() == "Grayscale Image")
					{
						isGrayscale = true;
					}
					string size = variablesDataView.Rows[vartoadd.RowNumber].Cells[2].Value.ToString();
					if (!size.Contains("x"))
					{
						vartoadd.Value = "Error: Size incorrect input use WidthxHeight";
						return vartoadd;
					}

					int xIndex = size.IndexOf('x');
					int width = Convert.ToInt16(size.Substring(0, xIndex));
					int height = Convert.ToInt16(size.Substring(xIndex + 1));
					Bitmap screen = new Bitmap(width, height);
					int row = 0, col = 0, grayscale = (width / 8) * height;
					if (width % 8 != 0)
					{
						MessageBox.Show("Width is not a multiple of 8!");
						return vartoadd;
					}

					for (int i = 0; i < width / 8 * height; i++)
					{
						int anotherbyte = 0;
						int abyte = _debugger.ReadByte((ushort)(vartoadd.Address + i));
						if (isGrayscale)
						{
							anotherbyte = _debugger.ReadByte((ushort)(vartoadd.Address + grayscale + i));
						}
						for (int bit = 128; bit > 0; bit /= 2)
						{
							if (isGrayscale)
							{
								if ((abyte & bit) != 0 && (anotherbyte & bit) != 0)
								{
									screen.SetPixel(col, row, Color.Black);
								}
								else if ((abyte & bit) != 0 && (anotherbyte & bit) == 0)
								{
									screen.SetPixel(col, row, Color.DarkGray);
								}
								else if ((abyte & bit) == 0 && (anotherbyte & bit) != 0)
								{
									screen.SetPixel(col, row, Color.LightGray);
								}
							}
							else if ((abyte & bit) != 0)
							{
								screen.SetPixel(col, row, Color.Black);
							}
							col++;
						}

						if (col < width)
						{
							continue;
						}
						col = 0;
						row++;
					}

					vartoadd.Value = screen.ResizeImage(width * 2, height * 2);
					break;
				case "String":
					vartoadd.Value = String.Empty;
					if (vartoadd.NumBytes == -1)
					{
						int i = 0;
						char charToAdd;
						do
						{
							charToAdd = (char)_debugger.ReadByte((ushort)(vartoadd.Address + i));
							vartoadd.Value += charToAdd.ToString();
							i++;
						}
						while (charToAdd != '\0');
					}
					else
					{
						for (int i = 0; i < vartoadd.NumBytes; i++)
						{
							vartoadd.Value += ((char)_debugger.ReadByte((ushort)(vartoadd.Address + i))).ToString();
						}
					}

					break;
				case "Word":
					int convertMethod = 16;
					vartoadd.Value = String.Empty;
					if (variablesDataView.Rows[vartoadd.RowNumber].Cells[4].Value == null)
					{
						variablesDataView.Rows[vartoadd.RowNumber].Cells[4].Value = 0;
					}
					switch (variablesDataView.Rows[vartoadd.RowNumber].Cells[4].Value.ToString())
					{
						case "Decimal":
							convertMethod = 10;
							break;
						case "Binary":
							convertMethod = 2;
							break;
						case "Octal":
							convertMethod = 8;
							break;
					}

					for (int i = 0; i < vartoadd.NumBytes * 2; i += 2)
					{
						byte baseValue = _debugger.ReadByte((ushort)(vartoadd.Address + i + 1));
						byte baseValue2 = _debugger.ReadByte((ushort)(vartoadd.Address + i));
						string value;
						string value2;
						switch (variablesDataView.Rows[vartoadd.RowNumber].Cells[4].Value.ToString())
						{
							case "Decimal":
								vartoadd.Value += Convert.ToString(baseValue, convertMethod) + Convert.ToString(baseValue2, convertMethod) + " ";
								break;
							case "Binary":
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 8)
								{
									value = "0" + value;
								}
								value2 = Convert.ToString(baseValue2, convertMethod);
								while (value2.Length < 8)
								{
									value2 = "0" + value2;
								}
								vartoadd.Value += value + value2 + " ";
								break;
							case "Octal":
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 4)
								{
									value = "0" + value;
								}
								value2 = Convert.ToString(baseValue2, convertMethod);
								while (value2.Length < 4)
								{
									value2 = "0" + value2;
								}
								vartoadd.Value += value + value2 + " ";
								break;
							default:
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 2)
								{
									value = "0" + value;
								}
								value2 = Convert.ToString(baseValue2, convertMethod);
								while (value2.Length < 2)
								{
									value2 = "0" + value2;
								}
								vartoadd.Value += value + value2 + " ";
								break;
						}
					}

					break;
				default:
					vartoadd.Value = String.Empty;
					if (variablesDataView.Rows[vartoadd.RowNumber].Cells[4].Value == null)
					{
						variablesDataView.Rows[vartoadd.RowNumber].Cells[4].Value = 0;
					}

					switch (variablesDataView.Rows[vartoadd.RowNumber].Cells[4].Value.ToString())
					{
						case "Decimal":
							convertMethod = 10;
							break;
						case "Binary":
							convertMethod = 2;
							break;
						case "Octal":
							convertMethod = 8;
							break;
						default:
							convertMethod = 16;
							break;
					}

					for (int i = 0; i < vartoadd.NumBytes; i++)
					{
						byte baseValue = _debugger.ReadByte((ushort)(vartoadd.Address + i));
						string value;
						switch (variablesDataView.Rows[vartoadd.RowNumber].Cells[4].Value.ToString())
						{
							case "Decimal":
								vartoadd.Value += Convert.ToString(baseValue, convertMethod) + " ";
								break;
							case "Binary":
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 8)
								{
									value = "0" + value;
								}
								vartoadd.Value += value + " ";
								break;
							case "Octal":
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 4)
								{
									value = "0" + value;
								}
								vartoadd.Value += value + " ";
								break;
							default:
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 2)
								{
									value = "0" + value;
								}
								vartoadd.Value += value + " ";
								break;
						}
					}

					break;
			}

			return vartoadd;
		}

		private void temp_FormClosed(object sender, FormClosedEventArgs e)
		{
			foreach (BitmapViewer test in _openBitmaps)
			{
				if (test.Tag.ToString() == ((BitmapViewer)sender).Tag.ToString())
				{
					_openBitmaps.Remove(test);
					break;
				}
			}
		}

		private void variablesDataView_CellContentDoubleClick(object sender, DataGridViewCellEventArgs e)
		{
			if (e.ColumnIndex != 5 || !variablesDataView[e.ColumnIndex, e.RowIndex].Value.ToString().Contains("Double click"))
			{
				return;
			}

			if (_openBitmaps.Any(test => test.Tag.ToString() == e.RowIndex.ToString()))
			{
				return;
			}

			BitmapViewer temp = new BitmapViewer((Image)_variables[e.RowIndex].Value)
			{
				Tag = e.RowIndex
			};
			temp.Show();
			_openBitmaps.Add(temp);
			temp.FormClosed += temp_FormClosed;
		}

		private void variablesDataView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
		{
			if (_variables.Count == 0 || variablesDataView.Rows[e.RowIndex].Cells[3].Value == null || e.RowIndex >= _variables.Count || variablesDataView.Rows[e.RowIndex].Cells[1].Value == null)
			{
				return;
			}
			((DataGridViewComboBoxCell)variablesDataView.Rows[e.RowIndex].Cells[4]).Items.Clear();
			switch (variablesDataView.Rows[e.RowIndex].Cells[3].Value.ToString())
			{
				case "Image":
					((DataGridViewComboBoxCell)variablesDataView.Rows[e.RowIndex].Cells[4]).Items.Add("B&W Image");
					((DataGridViewComboBoxCell)variablesDataView.Rows[e.RowIndex].Cells[4]).Items.Add("Grayscale Image");
					break;
				case "Byte":
				case "Word":
					((DataGridViewComboBoxCell)variablesDataView.Rows[e.RowIndex].Cells[4]).Items.Add("Binary");
					((DataGridViewComboBoxCell)variablesDataView.Rows[e.RowIndex].Cells[4]).Items.Add("Octal");
					((DataGridViewComboBoxCell)variablesDataView.Rows[e.RowIndex].Cells[4]).Items.Add("Decimal");
					((DataGridViewComboBoxCell)variablesDataView.Rows[e.RowIndex].Cells[4]).Items.Add("Hexadecimal");
					break;
				case "String":
					((DataGridViewComboBoxCell)variablesDataView.Rows[e.RowIndex].Cells[4]).Items.Add("String");
					break;
			}

			bool error = false;
			DebugVariable vartoadd = _variables[e.RowIndex];
			string labelOrAddress = variablesDataView.Rows[e.RowIndex].Cells[1].Value.ToString().Trim();
			ushort offset = 0;
			if (labelOrAddress.Contains("+"))
			{
				error |= !ushort.TryParse(labelOrAddress.Substring(labelOrAddress.IndexOf('+') + 1, labelOrAddress.Length - labelOrAddress.IndexOf('+') - 1), out offset);
				labelOrAddress = labelOrAddress.Substring(0, labelOrAddress.IndexOf('+'));
			}

			if (!error)
			{
				if (!Settings.Default.caseSensitive)
				{
					labelOrAddress = labelOrAddress.ToUpper();
				}
				string address = _symbolService.SymbolTable.GetAddressFromLabel(labelOrAddress);
				if (address != null)
				{
					vartoadd.Address = (ushort)(ushort.Parse(address.Substring(1, 4), NumberStyles.HexNumber) + offset);
				}
				else if ("bc'de'hl'spixiypc".IndexOf(address.ToLower()) != -1)
				{
					switch (address.ToLower())
					{
						case "hl":
							vartoadd.Address = _debugger.CPU.HL;
							break;
						case "de":
							vartoadd.Address = _debugger.CPU.DE;
							break;
						case "bc":
							vartoadd.Address = _debugger.CPU.BC;
							break;
						case "ix":
							vartoadd.Address = _debugger.CPU.IX;
							break;
						case "iy":
							vartoadd.Address = _debugger.CPU.IY;
							break;
						case "sp":
							vartoadd.Address = _debugger.CPU.SP;
							break;
						case "pc":
							vartoadd.Address = _debugger.CPU.PC;
							break;
					}

				}
				else
				{
					ushort addr;
					error = !ushort.TryParse(labelOrAddress, NumberStyles.HexNumber, null, out addr);
					vartoadd.Address = addr;
					if (variablesDataView.Rows[e.RowIndex].Cells[1].Value.ToString().Contains("gay"))
					{
						vartoadd.Value = "Spencer is gayer";
					}
				}
			}
			if (variablesDataView.Rows[e.RowIndex].Cells[2].Value != null)
			{
				if (!variablesDataView.Rows[e.RowIndex].Cells[2].Value.ToString().Contains("x"))
				{
					int numBytes;
					error |= !int.TryParse(variablesDataView.Rows[e.RowIndex].Cells[2].Value.ToString(), out numBytes);
					vartoadd.NumBytes = numBytes;
				}
			}
			else
			{
				error = true;
			}
			if (variablesDataView.Rows[e.RowIndex].Cells[3].Value == null)
			{
				variablesDataView.Rows[e.RowIndex].Cells[3].Value = "Byte";
			}
			if (!error)
			{
				vartoadd = GetValue(vartoadd);
			}
			else if (!vartoadd.Value.ToString().Contains("gay"))
			{
				vartoadd.Value = "Error";
			}
			var value = vartoadd.Value as Bitmap;
			if (value != null)
			{
				variablesDataView.Rows[e.RowIndex].Cells[5].Value = "Double click for screen";
				foreach (BitmapViewer test in _openBitmaps.Where(test => test.Tag.ToString() == e.RowIndex.ToString()))
				{
					test.pictureBox.Image = value;
				}
			}
			else
			{
				variablesDataView.Rows[e.RowIndex].Cells[5].Value = vartoadd.Value;
			}
			_variables[e.RowIndex] = vartoadd;
		}

		private void variablesDataView_CellEnter(object sender, DataGridViewCellEventArgs e)
		{
			if (variablesDataView[e.ColumnIndex, e.RowIndex].GetType() == typeof(DataGridViewComboBoxCell))
			{
				SendKeys.Send("{F4}");
			}
		}

		private void variablesDataView_ColumnDisplayIndexChanged(object sender, DataGridViewColumnEventArgs e)
		{
			MessageBox.Show("test");
		}

		private void variablesDataView_DataError(object sender, DataGridViewDataErrorEventArgs e)
		{
		}

		private void variablesDataView_RowsAdded(object sender, DataGridViewRowsAddedEventArgs e)
		{
			DebugVariable vartoadd = new DebugVariable { Address = 0, NumBytes = 1, RowNumber = e.RowIndex - 1 };
			vartoadd.Value = _debugger.ReadByte(vartoadd.Address).ToString("X" + vartoadd.NumBytes * 2);
			_variables.Add(vartoadd);
			variablesDataView.Rows[e.RowIndex].Cells[1].Value = vartoadd.Address;
			variablesDataView.Rows[e.RowIndex].Cells[2].Value = vartoadd.NumBytes;
		}

		private void variablesDataView_RowsRemoved(object sender, DataGridViewRowsRemovedEventArgs e)
		{
			_variables.Remove(_variables[e.RowIndex]);
			for (int i = 0; i < _variables.Count; i++)
			{
				DebugVariable var = _variables[i];
				if (var.RowNumber >= e.RowIndex)
				{
					var.RowNumber--;
				}
				_variables[i] = var;
			}
		}

		private void variablesDataView_UserAddedRow(object sender, DataGridViewRowEventArgs e)
		{
			// addNewVariable(vartoadd);
		}

		private struct DebugVariable
		{
			// public string name;
			public ushort Address { get; set; }
			public int NumBytes { get; set; }
			public int RowNumber { get; set; }
			public object Value { get; set; }
		}
	}
}