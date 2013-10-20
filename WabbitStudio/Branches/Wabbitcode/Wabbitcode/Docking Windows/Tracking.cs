using System.Linq;
using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services.Interface;

namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class TrackingWindow : ToolWindow
	{
		private readonly List<BitmapViewer> _openBitmaps = new List<BitmapViewer>();
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

        #region Clipboard Operation

        public override void Copy()
		{
		    if (variablesDataView == null)
		    {
		        return;
		    }

		    var dataObject = variablesDataView.GetClipboardContent();
		    if (dataObject != null)
		    {
		        Clipboard.SetDataObject(dataObject);
		    }
		}

		public override void Paste()
		{
			if (variablesDataView.EditingControl != null)
			{
				variablesDataView.EditingControl.Text = Clipboard.GetData(DataFormats.Text).ToString();
			}
		}

        #endregion

        #region Parsing

        private static bool Predecessor(string firstOperator, string secondOperator)
        {
            const string opString = "(+-*/%";

            int[] precedence = { 0, 12, 12, 13, 13, 13 };

            int firstPoint = opString.IndexOf(firstOperator);
            int secondPoint = opString.IndexOf(secondOperator);

            return precedence[firstPoint] >= precedence[secondPoint];
        }

        private static IEnumerable<string> InfixToPostfix(IEnumerable<string> infixArray)
        {
            var stack = new Stack<string>();
            var postfix = new Stack<string>();

            foreach (string op in infixArray)
            {
                if (!("()*/+-%".Contains(op)))
                {
                    postfix.Push(op);
                }
                else
                {
                    if (op.Equals("("))
                    {
                        stack.Push("(");
                    }
                    else
                    {
                        string st;
                        if (op.Equals(")"))
                        {
                            st = stack.Pop();
                            while (!(st.Equals("(")))
                            {
                                postfix.Push(st);
                                st = stack.Pop();
                            }
                        }
                        else
                        {
                            while (stack.Count > 0)
                            {
                                st = stack.Pop();
                                if (Predecessor(st, op))
                                {
                                    postfix.Push(st);
                                }
                                else
                                {
                                    stack.Push(st);
                                    break;
                                }
                            }
                            stack.Push(op);
                        }
                    }
                }
            }
            while (stack.Count > 0)
            {
                postfix.Push(stack.Pop());
            }

            return new Stack<string>(postfix);
        }

	    private int EvaluateElement(string element)
	    {
	        if (!Settings.Default.caseSensitive)
	        {
	            element = element.ToUpper();
	        }

            int value;
            if (int.TryParse(element, out value))
            {
                return value;
            }

	        string label = _symbolService.SymbolTable.GetAddressFromLabel(element);
	        if (label != null)
	        {
	            return int.Parse(label, NumberStyles.HexNumber);
	        }

	        switch (element)
	        {
                case "$A":
	                return _debugger.CPU.A;
                case "$F":
                    return _debugger.CPU.F;
                case "$B":
                    return _debugger.CPU.B;
                case "$C":
                    return _debugger.CPU.C;
                case "$D":
                    return _debugger.CPU.D;
                case "$E":
                    return _debugger.CPU.E;
                case "$H":
                    return _debugger.CPU.H;
                case "$L":
                    return _debugger.CPU.L;
                case "$IXH":
                    return _debugger.CPU.IXH;
                case "$IXL":
                    return _debugger.CPU.IXL;
                case "$IYH":
                    return _debugger.CPU.IYH;
                case "$IYL":
                    return _debugger.CPU.IYL;
                case "$AF":
                    return _debugger.CPU.AF;
                case "$BC":
                    return _debugger.CPU.BC;
                case "$DE":
                    return _debugger.CPU.DE;
                case "$HL":
                    return _debugger.CPU.HL;
                case "$IX":
                    return _debugger.CPU.IX;
                case "$IY":
	                return _debugger.CPU.IY;
                case "$PC":
                    return _debugger.CPU.PC;
                case "$SP":
                    return _debugger.CPU.SP;
	        }
	        throw new FormatException(string.Format("Can't recognize symbol {0}", element));
	    }

	    private int EvaluateElements(string right, string left, string op)
	    {
	        int leftVal = EvaluateElement(right);
	        int rightVal = EvaluateElement(left);
	        switch (op)
	        {
                case "+":
	                return leftVal + rightVal;
                case "-":
	                return leftVal - rightVal;
                case "*":
	                return leftVal * rightVal;
                case "/":
	                return leftVal / rightVal;
                case "%":
	                return leftVal % rightVal;
	        }
            throw new Exception("Invalid operator");
	    }

        private int EvalPostfix(IEnumerable<string> postFix)
        {
            Stack<string> parseStack = new Stack<string>();
            foreach (string element in postFix)
            {
                if (element == "+" || element == "-" || element == "*" || element == "/")
                {
                    if (parseStack.Count < 2)
                    {
                        throw new FormatException("Invalid syntax");
                    }

                    string left = parseStack.Pop();
                    string right = parseStack.Pop();
                    int eval = EvaluateElements(right, left, element);
                    parseStack.Push(eval.ToString());
                }
                else
                {
                    parseStack.Push(element);
                }
            }

            return EvaluateElement(parseStack.Pop());
        }

        #endregion

        #region Row Data

        private static string GetRowAddress(DataGridViewRow row)
        {
            return row.Cells[1].Value != null ? row.Cells[1].Value.ToString() : null;
        }

        private static string GetRowNumBytes(DataGridViewRow row)
        {
            if (row.Cells[2].Value == null)
            {
                row.Cells[2].Value = "1";
            }

            return row.Cells[2].Value.ToString();
        }

        private static VariableType GetRowVarType(DataGridViewRow row)
        {
            DataGridViewComboBoxCell cell = (DataGridViewComboBoxCell)row.Cells[3];
            if (cell.Value == null)
            {
                cell.Items.Clear();
                cell.Items.Add(VariableType.Byte);
                cell.Items.Add(VariableType.Word);
                cell.Items.Add(VariableType.Image);
                cell.Items.Add(VariableType.String);
                cell.Value = VariableType.Byte;
            }

            return (VariableType) Enum.Parse(typeof(VariableType), cell.Value.ToString());
        }

        private static VariableDisplayMethod GetRowVarDisplayMethod(DataGridViewRow row)
        {
            DataGridViewComboBoxCell cell = (DataGridViewComboBoxCell)row.Cells[4];
            cell.Items.Clear();
            switch (GetRowVarType(row))
            {
                case VariableType.Byte:
                case VariableType.Word:
                    cell.Items.Add(VariableDisplayMethod.Hexadecimal);
                    cell.Items.Add(VariableDisplayMethod.Decimal);
                    cell.Items.Add(VariableDisplayMethod.Binary);
                    cell.Items.Add(VariableDisplayMethod.Octal);
                    cell.Value = VariableDisplayMethod.Hexadecimal;
                    break;
                case VariableType.Image:
                    cell.Items.Add(VariableDisplayMethod.BlackWhiteImage);
                    cell.Items.Add(VariableDisplayMethod.GrayscaleImage);
                    cell.Value = VariableDisplayMethod.BlackWhiteImage;
                    break;
                case VariableType.String:
                    cell.Items.Add(VariableDisplayMethod.String);
                    cell.Value = VariableDisplayMethod.String;
                    break;
                default:
                    throw new Exception("Invalid var type");
            }

            return (VariableDisplayMethod)Enum.Parse(typeof(VariableDisplayMethod), cell.Value.ToString());
        }

        private void SetRowValue(DataGridViewRow row, object cellData)
        {
            row.Cells[5].Value = cellData;
        }

        #endregion

        private int GetAddressValue(string address)
        {
            Regex re = new Regex(@"([\+\-\*\(\)\^\/\ ])", RegexOptions.Compiled);
            if (string.IsNullOrEmpty(address))
            {
                throw new FormatException("Missing address value");
            }

            var tokenList = re.Split(address).Select(t => t.Trim()).Where(t => !string.IsNullOrEmpty(t));
            var postFix = InfixToPostfix(tokenList);
            int value = EvalPostfix(postFix);
            return value;
        }

	    private void UpdateRowValue(DataGridViewRow row)
	    {
	        int address;
	        int numBytes;
	        try
	        {
	            address = GetAddressValue(GetRowAddress(row));
	        }
	        catch (FormatException ex)
	        {
	            row.Cells[5].Value = string.Format("Error: {0}", ex.Message);
	            return;
	        }

	        if (!int.TryParse(GetRowNumBytes(row), out numBytes))
	        {
	            SetRowValue(row, "Error: missing number of bytes");
	            return;
	        }

            object value = GetValue(row, address, numBytes);
            SetRowValue(row, value);
	    }

	    private object GetValue(DataGridViewRow dataRow, int address, int numBytes)
	    {
	        string outputValue;
	        string value = string.Empty;
	        VariableType valueTypeEntry = GetRowVarType(dataRow);
            VariableDisplayMethod method = GetRowVarDisplayMethod(dataRow);
			switch (valueTypeEntry)
			{
				case VariableType.Image:
			        bool isGrayscale = GetRowVarDisplayMethod(dataRow) == VariableDisplayMethod.GrayscaleImage;
			        string size = GetRowNumBytes(dataRow);
					if (!size.Contains("x"))
					{
						return "Error: Size incorrect input use Width x Height";
					}

					int xIndex = size.IndexOf('x');
					int width = Convert.ToInt16(size.Substring(0, xIndex));
					int height = Convert.ToInt16(size.Substring(xIndex + 1));
					Bitmap screen = new Bitmap(width, height);
					int row = 0, col = 0, grayscale = (width / 8) * height;
					if (width % 8 != 0)
					{
						return "Width is not a multiple of 8!";
					}

					for (int i = 0; i < width / 8 * height; i++)
					{
						int anotherbyte = 0;
						int abyte = _debugger.ReadByte((ushort)(address + i));
						if (isGrayscale)
						{
							anotherbyte = _debugger.ReadByte((ushort)(address + grayscale + i));
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

                    return screen.ResizeImage(width * 2, height * 2);
				case VariableType.String:
					outputValue = string.Empty;
					if (numBytes == -1)
					{
						int i = 0;
						char charToAdd;
						do
						{
							charToAdd = (char)_debugger.ReadByte((ushort)(address + i));
							outputValue += charToAdd.ToString();
							i++;
						}
						while (charToAdd != '\0');
					}
					else
					{
						for (int i = 0; i < numBytes; i++)
						{
							outputValue += ((char)_debugger.ReadByte((ushort)(address + i))).ToString();
						}
					}

					break;
				case VariableType.Word:
					int convertMethod;
					outputValue = string.Empty;
					switch (method)
					{
						case VariableDisplayMethod.Decimal:
							convertMethod = 10;
							break;
						case VariableDisplayMethod.Binary:
							convertMethod = 2;
							break;
						case VariableDisplayMethod.Octal:
							convertMethod = 8;
							break;
                        case VariableDisplayMethod.Hexadecimal:
					        convertMethod = 16;
					        break;
                        default:
                            throw new Exception("Invalid display method");
					}

					for (int i = 0; i < numBytes * 2; i += 2)
					{
						byte baseValue = _debugger.ReadByte((ushort)(address + i + 1));
						byte baseValue2 = _debugger.ReadByte((ushort)(address + i));
					    string value2;
						switch (method)
						{
							case VariableDisplayMethod.Decimal:
								value += Convert.ToString(baseValue, convertMethod) + Convert.ToString(baseValue2, convertMethod) + " ";
								break;
                            case VariableDisplayMethod.Binary:
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
								outputValue += value + value2 + " ";
								break;
                            case VariableDisplayMethod.Octal:
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
								outputValue += value + value2 + " ";
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
						        outputValue += value + value2 + " ";
								break;
						}
					}

					break;
				default:
					outputValue = string.Empty;
					switch (method)
					{
						case VariableDisplayMethod.Decimal:
							convertMethod = 10;
							break;
						case VariableDisplayMethod.Binary:
							convertMethod = 2;
							break;
						case VariableDisplayMethod.Octal:
							convertMethod = 8;
							break;
                        case VariableDisplayMethod.Hexadecimal:
					        convertMethod = 16;
					        break;
                        default:
                            throw new Exception("Invalid display method");
					}

					for (int i = 0; i < numBytes; i++)
					{
						byte baseValue = _debugger.ReadByte((ushort)(address + i));
						switch (method)
						{
                            case VariableDisplayMethod.Decimal:
								outputValue += Convert.ToString(baseValue, convertMethod) + " ";
								break;
                            case VariableDisplayMethod.Binary:
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 8)
								{
									value = "0" + value;
								}
								outputValue += value + " ";
								break;
                            case VariableDisplayMethod.Octal:
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 4)
								{
									value = "0" + value;
								}
								outputValue += value + " ";
								break;
							default:
								value = Convert.ToString(baseValue, convertMethod);
								while (value.Length < 2)
								{
									value = "0" + value;
								}
								outputValue += value + " ";
								break;
						}
					}
					break;
			}

			return outputValue;
		}

		private void temp_FormClosed(object sender, FormClosedEventArgs e)
		{
		    foreach (BitmapViewer test in _openBitmaps.Where(test => test.Tag == ((Form)sender).Tag))
		    {
		        _openBitmaps.Remove(test);
		        break;
		    }
		}

	    private void variablesDataView_CellContentDoubleClick(object sender, DataGridViewCellEventArgs e)
		{
			if (e.ColumnIndex != 5 || !variablesDataView[e.ColumnIndex, e.RowIndex].Value.ToString().Contains("Double click"))
			{
				return;
			}

			if (_openBitmaps.Any(test => (int) test.Tag == e.RowIndex))
			{
				return;
			}

            //BitmapViewer temp = new BitmapViewer((Image)_variables[e.RowIndex].Value)
            //{
            //    Tag = e.RowIndex
            //};
            //temp.Show();
            //_openBitmaps.Add(temp);
            //temp.FormClosed += temp_FormClosed;
		}

		private void variablesDataView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
		{
			this.Invoke(() => UpdateRowValue(variablesDataView.Rows[e.RowIndex]));
		}

	    private void variablesDataView_CellEnter(object sender, DataGridViewCellEventArgs e)
		{
			if (variablesDataView[e.ColumnIndex, e.RowIndex] is DataGridViewComboBoxCell)
			{
				SendKeys.Send("{F4}");
			}
		}

	    private enum VariableDisplayMethod
        {
            BlackWhiteImage,
            GrayscaleImage,
            Binary,
            Octal,
            Decimal,
            Hexadecimal,
            String,
        }

	    private enum VariableType
        {
            Byte,
            Word,
            Image,
            String,
        }

        private void variablesDataView_DataError(object sender, DataGridViewDataErrorEventArgs e)
        {
            var test = e.ThrowException;
        }
	}
}