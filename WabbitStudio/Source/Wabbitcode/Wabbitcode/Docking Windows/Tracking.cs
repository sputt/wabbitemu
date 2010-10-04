using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Microsoft.CSharp;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class TrackingWindow : ToolWindow
    {
        private readonly List<debugVariable> variables = new List<debugVariable>();
        public TrackingWindow()
        {
            InitializeComponent();
        }
        
        struct debugVariable {
            public int rowNumber;
            //public string name;
            public ushort address;
            public int numBytes;
            public object value; 
        }

        public void updateVars()
        {
            for (int i = 0; i < variables.Count; i++)
            {
                debugVariable var = variables[i];
                if (var.rowNumber > variablesDataView.Rows.Count)
                    continue;
                //if (variablesDataView.Rows[var.rowNumber].Cells[0].Value != null)
                //    var.name = variablesDataView.SelectedRows[0].Cells[0].Value.ToString();
                if (var.value.ToString() == "Error")
                    continue;
                var = getValue(var);
                if (var.value.GetType() == typeof(Bitmap))
                {
                    variablesDataView.Rows[var.rowNumber].Cells[5].Value = "Double click for screen";
                    foreach (BitmapViewer test in openBitmaps)
                        if (test.Tag.ToString() == var.rowNumber.ToString())
                            test.pictureBox.Image = (Image)var.value;
                }
                else
                    variablesDataView.Rows[var.rowNumber].Cells[5].Value = var.value;
                variables[i] = var;
            }
        }

        private void variablesDataView_UserAddedRow(object sender, DataGridViewRowEventArgs e)
        {
            
            //addNewVariable(vartoadd);
        }

        private void variablesDataView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
			if (variables.Count == 0 || variablesDataView.Rows[e.RowIndex].Cells[3].Value == null || e.RowIndex >= variables.Count || variablesDataView.Rows[e.RowIndex].Cells[1].Value == null)
                return;
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
            debugVariable vartoadd = variables[e.RowIndex];
            string address = variablesDataView.Rows[e.RowIndex].Cells[1].Value.ToString().Trim();
            if (address != null)
            {
                ushort offset = 0;
                if (address.Contains("+"))
                {
                    error |= !ushort.TryParse( address.Substring(address.IndexOf('+') + 1, address.Length - address.IndexOf('+') - 1), out offset);
                    address = address.Substring(0, address.IndexOf('+'));
                }
                if (!error)
                {
                    if (!Settings.Default.caseSensitive)
                        address = address.ToUpper();
                    if (DebuggerService.SymbolTable.StaticLabels.Contains(address))
                        vartoadd.address =(ushort) (ushort.Parse(DebuggerService.SymbolTable.StaticLabels[address].ToString().
                                              Substring(1, 4), System.Globalization.NumberStyles.HexNumber) + offset);
					else if ("bc'de'hl'spixiypc".IndexOf(address.ToLower()) != -1)
					{
                        switch (address.ToLower())
                        {
#if NEW_DEBUGGING
                            case "hl":
                                vartoadd.address = DebuggerService.Debugger.CPU.HL;
                                break;
                            case "de":
                                vartoadd.address = DebuggerService.Debugger.CPU.DE;
                                break;
                            case "bc":
                                vartoadd.address = DebuggerService.Debugger.CPU.BC;
                                break;
                            case "ix":
                                vartoadd.address = DebuggerService.Debugger.CPU.IX;
                                break;
                            case "iy":
                                vartoadd.address = DebuggerService.Debugger.CPU.IY;
                                break;
                            case "sp":
                                vartoadd.address = DebuggerService.Debugger.CPU.SP;
                                break;
                            case "pc":
                                vartoadd.address = DebuggerService.Debugger.CPU.PC;
                                break;
#else

                            case "hl":
								vartoadd.address = DebuggerService.Debugger.getState().HL;
								break;
							case "de":
								vartoadd.address = DebuggerService.Debugger.getState().DE;
								break;
							case "bc":
								vartoadd.address = DebuggerService.Debugger.getState().BC;
								break;
							case "ix":
								vartoadd.address = DebuggerService.Debugger.getState().IX;
								break;
							case "iy":
								vartoadd.address = DebuggerService.Debugger.getState().IY;
								break;
							case "sp":
								vartoadd.address = DebuggerService.Debugger.getState().SP;
								break;
							case "pc":
								vartoadd.address = DebuggerService.Debugger.getState().PC;
								break;
#endif
                        }
						
					}
					else
                    {
                        error = !ushort.TryParse(variablesDataView.Rows[e.RowIndex].Cells[1].Value.ToString(),
                                                 System.Globalization.NumberStyles.HexNumber, null, out vartoadd.address);
                        if (variablesDataView.Rows[e.RowIndex].Cells[1].Value.ToString().Contains("gay"))
                            vartoadd.value = "Spencer is gayer";
                    }
                }
            }
            else
                error = true;
            if (variablesDataView.Rows[e.RowIndex].Cells[2].Value != null)
            {
                if (!variablesDataView.Rows[e.RowIndex].Cells[2].Value.ToString().Contains("x"))
                    error |= !int.TryParse(variablesDataView.Rows[e.RowIndex].Cells[2].Value.ToString(), out vartoadd.numBytes);

            }
            else
                error = true;
            if (variablesDataView.Rows[e.RowIndex].Cells[3].Value == null)
                variablesDataView.Rows[e.RowIndex].Cells[3].Value = "Byte";
            if (!error)
                vartoadd = getValue(vartoadd);
            else if (!vartoadd.value.ToString().Contains("gay"))
                vartoadd.value = "Error";
			if (vartoadd.value.GetType() == typeof(Bitmap))
			{
				variablesDataView.Rows[e.RowIndex].Cells[5].Value = "Double click for screen";
				foreach (BitmapViewer test in openBitmaps)
					if (test.Tag.ToString() == e.RowIndex.ToString())
						test.pictureBox.Image = (Image)vartoadd.value;
			}
			else
				variablesDataView.Rows[e.RowIndex].Cells[5].Value = vartoadd.value;
            variables[e.RowIndex] = vartoadd;
        }

        [DllImport("libWabbitemu.dll")]
        private static extern byte ReadMem(int slot, ushort address);

        private debugVariable getValue(debugVariable vartoadd)
        {
            if (vartoadd.rowNumber > variablesDataView.Rows.Count)
                return vartoadd;
            if (variablesDataView.Rows[vartoadd.rowNumber].Cells[3].Value == null)
                variablesDataView.Rows[vartoadd.rowNumber].Cells[3].Value = "Byte";
            string valueTypeEntry = variablesDataView.Rows[vartoadd.rowNumber].Cells[3].Value.ToString();
			switch (valueTypeEntry)
            {
				case "Image":
					bool isGrayscale = false;
					if (variablesDataView.Rows[vartoadd.rowNumber].Cells[4].Value == null)
						variablesDataView.Rows[vartoadd.rowNumber].Cells[4].Value = 0;
					if (variablesDataView.Rows[vartoadd.rowNumber].Cells[4].Value.ToString() == "Grayscale Image")
						isGrayscale = true;	
                    string size = variablesDataView.Rows[vartoadd.rowNumber].Cells[2].Value.ToString();
                    if (!size.Contains("x"))
                    {
                        vartoadd.value = "Error: Size incorrect input use WidthxHeight";
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
#if NEW_DEBUGGING
                        int anotherbyte = 0;
                        int abyte = DebuggerService.Debugger.Read((ushort)(vartoadd.address + i));
                        if (isGrayscale)
                            anotherbyte = DebuggerService.Debugger.Read((ushort)(vartoadd.address + grayscale + i));
#else
                        int anotherbyte = 0;
                        int abyte = DebuggerService.Debugger.readMem((ushort)(vartoadd.address + i));
                        if (isGrayscale)
                            anotherbyte = DebuggerService.Debugger.readMem((ushort)(vartoadd.address + grayscale + i));
#endif
                        /*while (abyte.Length < 8)
                            abyte = '0' + abyte;
						while (anotherbyte.Length < 8)
							anotherbyte = '0' + anotherbyte;*/
                        for (int bit = 128; bit > 0; bit /= 2)
                        {
							if (isGrayscale)
							{
								if ((abyte & bit) != 0 && (anotherbyte & bit) != 0)
									screen.SetPixel(col, row, Color.Black);
                                else if ((abyte & bit) != 0 && (anotherbyte & bit) == 0)
									screen.SetPixel(col, row, Color.DarkGray);
                                else if ((abyte & bit) == 0 && (anotherbyte & bit) != 0)
									screen.SetPixel(col, row, Color.LightGray);
							}
                            else if ((abyte & bit) != 0)
								screen.SetPixel(col, row, Color.Black);
                            col++;
                        }
                        if (col < width)
                            continue;
                        col = 0;
                        row++;
                    }
                    vartoadd.value = screen.ResizeImage(width * 2, height * 2);
                    break;
                case "String":
                    vartoadd.value = "";
					if (vartoadd.numBytes == -1)
					{
						int i = 0;
						char charToAdd;
						do
						{
#if NEW_DEBUGGING
                            charToAdd = (char) DebuggerService.Debugger.Read((ushort)(vartoadd.address + i));
#else
							charToAdd = (char) DebuggerService.Debugger.readMem((ushort)(vartoadd.address + i));
#endif
                            vartoadd.value += charToAdd.ToString();
							i++;
						} while (charToAdd != '\0');
					}
					else
                    {
#if NEW_DEBUGGING
                        vartoadd.value = String.Join(" ", DebuggerService.Debugger.Read(vartoadd.address, vartoadd.numBytes));
#else
                        for (int i = 0; i < vartoadd.numBytes; i++)
							vartoadd.value += ((char)DebuggerService.Debugger.readMem((ushort)(vartoadd.address + i))).ToString();
#endif
                    }
                    break;
                case "Word":
                    int convertMethod = 16;
                    vartoadd.value = "";
					if (variablesDataView.Rows[vartoadd.rowNumber].Cells[4].Value == null)
						variablesDataView.Rows[vartoadd.rowNumber].Cells[4].Value = 0;
					switch(variablesDataView.Rows[vartoadd.rowNumber].Cells[4].Value.ToString())
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
                    for (int i = 0; i < vartoadd.numBytes * 2; i+=2)
					{
#if NEW_DEBUGGING
                        vartoadd.value += ReadMem(0, (ushort)(vartoadd.address + i + 1)).ToString("X2") + ReadMem(0, (ushort)(vartoadd.address + i)).ToString("X2") + " ";
#else
                        byte baseValue = DebuggerService.Debugger.readMem((ushort)(vartoadd.address + i + 1));
						byte baseValue2 = DebuggerService.Debugger.readMem((ushort)(vartoadd.address + i));
						string value;
						string value2;
						switch ((variablesDataView.Rows[vartoadd.rowNumber].Cells[4].Value.ToString()))
						{
							case "Decimal":
								vartoadd.value += Convert.ToString(baseValue, convertMethod) + Convert.ToString(baseValue2, convertMethod) + " ";
								break;
							case "Binary":
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 8)
									value = "0" + value;
								value2 = Convert.ToString(baseValue2, convertMethod);
								while (value2.Length < 8)
									value2 = "0" + value2;
								vartoadd.value += value + value2 + " ";
								break;
							case "Octal":
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 4)
									value = "0" + value;
								value2 = Convert.ToString(baseValue2, convertMethod);
								while (value2.Length < 4)
									value2 = "0" + value2;
								vartoadd.value += value + value2 + " ";
								break;
							default:
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 2)
									value = "0" + value;
								value2 = Convert.ToString(baseValue2, convertMethod);
								while (value2.Length < 2)
									value2 = "0" + value2;
								vartoadd.value += value + value2 + " ";
								break;
						}
#endif
					}
                    break;
                default:
                    vartoadd.value = "";
					if (variablesDataView.Rows[vartoadd.rowNumber].Cells[4].Value == null)
						variablesDataView.Rows[vartoadd.rowNumber].Cells[4].Value = 0;
					switch(variablesDataView.Rows[vartoadd.rowNumber].Cells[4].Value.ToString())
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
                    for (int i = 0; i < vartoadd.numBytes; i++)
					{
#if NEW_DEBUGGING
                        vartoadd.value += ReadMem(0, (ushort)(vartoadd.address + i)).ToString("X2") + " ";
#else
						byte baseValue = DebuggerService.Debugger.readMem((ushort)(vartoadd.address + i));
                        string value;
                        switch ((variablesDataView.Rows[vartoadd.rowNumber].Cells[4].Value.ToString()))
                        {
                            case "Decimal":
                                vartoadd.value += Convert.ToString(baseValue, convertMethod) + " ";
                                break;
                            case "Binary":
                                value = Convert.ToString(baseValue, convertMethod);
                                while (value.Length < 8)
                                    value = "0" + value;
                                vartoadd.value += value + " ";
                                break;
                            case "Octal":
                                value = Convert.ToString(baseValue, convertMethod);
                                while (value.Length < 4)
                                    value = "0" + value;
                                vartoadd.value += value  + " ";
                                break;
                            default:
                                value = Convert.ToString(baseValue, convertMethod);
                                while (value.Length < 2)
                                    value = "0" + value;
                                vartoadd.value += value + " ";
                                break;
                        }
#endif
					}
                    break;
            }
            return vartoadd;
        }

        private void variablesDataView_RowsAdded(object sender, DataGridViewRowsAddedEventArgs e)
        {
            debugVariable vartoadd = new debugVariable { address = 0, numBytes = 1, rowNumber = e.RowIndex - 1 };
#if NEW_DEBUGGING
            vartoadd.value = ReadMem(0, vartoadd.address).ToString("X" + vartoadd.numBytes * 2);
#else
            vartoadd.value = DebuggerService.Debugger.readMem(vartoadd.address).ToString("X" + vartoadd.numBytes * 2);
#endif
            variables.Add(vartoadd);
            variablesDataView.Rows[e.RowIndex].Cells[1].Value = vartoadd.address;
            variablesDataView.Rows[e.RowIndex].Cells[2].Value = vartoadd.numBytes;
        }

        private void variablesDataView_RowsRemoved(object sender, DataGridViewRowsRemovedEventArgs e)
        {
            variables.Remove(variables[e.RowIndex]);
			for (int i = 0; i < variables.Count; i++)
			{
				debugVariable var = variables[i];
				if (var.rowNumber >= e.RowIndex)
					var.rowNumber--;
				variables[i] = var;
			}
        }

        public override void Copy()
        {
            Clipboard.SetDataObject(variablesDataView.GetClipboardContent());
        }

        public override void Paste()
        {
            if (variablesDataView.EditingControl != null)
                variablesDataView.EditingControl.Text = Clipboard.GetData(DataFormats.Text).ToString();
        }

        private readonly List<	BitmapViewer> openBitmaps = new List<BitmapViewer>();
        private void variablesDataView_CellContentDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex != 5 || !variablesDataView[e.ColumnIndex, e.RowIndex].Value.ToString().Contains("Double click"))
                return;
            foreach(BitmapViewer test in openBitmaps)
                if (test.Tag.ToString() == e.RowIndex.ToString())
                    return;
            BitmapViewer temp = new BitmapViewer((Image) variables[e.RowIndex].value);
            temp.Tag = e.RowIndex;
            temp.Show();
            openBitmaps.Add(temp);
            temp.FormClosed += temp_FormClosed;
        }

        void temp_FormClosed(object sender, FormClosedEventArgs e)
        {
            foreach(BitmapViewer test in openBitmaps)
                if (test.Tag.ToString() == ((BitmapViewer)sender).Tag.ToString())
                {
                    openBitmaps.Remove(test);
                    break;
                }
        }

        private void variablesDataView_ColumnDisplayIndexChanged(object sender, DataGridViewColumnEventArgs e)
        {
            MessageBox.Show("test");
        }

		private void variablesDataView_CellEnter(object sender, DataGridViewCellEventArgs e)
		{
			if (variablesDataView[e.ColumnIndex, e.RowIndex].GetType() == typeof(DataGridViewComboBoxCell))
				SendKeys.Send("{F4}");

		}

		private void variablesDataView_DataError(object sender, DataGridViewDataErrorEventArgs e)
		{

		}
    }
}