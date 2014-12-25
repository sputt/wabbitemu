using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public partial class TrackingWindow : ToolWindow
    {
        private const int AddressIndex = 1;
        private const int NumBytesIndex = 2;
        private const int VarTypeIndex = 3;
        private const int VarDisplayMethodIndex = 4;
        private const int VarValueIndex = 5;

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

        private readonly List<BitmapViewer> _openBitmaps = new List<BitmapViewer>();
        private readonly Dictionary<int, Image> _imageList = new Dictionary<int, Image>();
        private readonly ExpressionEvaluator _expressionEvaluator;
        private IWabbitcodeDebugger _debugger;
        private VariableType _lastVariableType;

        public TrackingWindow()
        {
            InitializeComponent();

            IDebuggerService debuggerService = DependencyFactory.Resolve<IDebuggerService>();
            ISymbolService symbolService = DependencyFactory.Resolve<ISymbolService>();
            debuggerService.OnDebuggingStarted += DebuggerService_OnDebuggingStarted;
            _expressionEvaluator = new ExpressionEvaluator(symbolService, debuggerService);
        }

        private void DebuggerService_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            _debugger = e.Debugger;
            _debugger.DebuggerStep += (o, args) =>
            {
                UpdateAllRows();
                EnablePanel(true);
            };
            _debugger.DebuggerRunningChanged += (o, args) =>
            {
                if (!args.Running)
                {
                    UpdateAllRows();
                }
                EnablePanel(!args.Running);
            };
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

        #region Row Data

        private static string GetRowAddress(DataGridViewRow row)
        {
            return row.Cells[AddressIndex].Value != null ? row.Cells[AddressIndex].Value.ToString() : string.Empty;
        }

        private static string GetRowNumBytes(DataGridViewRow row)
        {
            if (row.Cells[NumBytesIndex].Value == null)
            {
                row.Cells[NumBytesIndex].Value = "1";
            }

            return row.Cells[NumBytesIndex].Value.ToString();
        }

        private static VariableType GetRowVarType(DataGridViewRow row)
        {
            DataGridViewComboBoxCell cell = (DataGridViewComboBoxCell) row.Cells[VarTypeIndex];
            if (cell.Value == null)
            {
                cell.Items.Clear();
                cell.Items.Add(VariableType.Byte.ToString());
                cell.Items.Add(VariableType.Word.ToString());
                cell.Items.Add(VariableType.Image.ToString());
                cell.Items.Add(VariableType.String.ToString());
                cell.Value = VariableType.Byte.ToString();
            }

            return (VariableType) Enum.Parse(typeof(VariableType), cell.Value.ToString());
        }

        private VariableDisplayMethod GetRowVarDisplayMethod(DataGridViewRow row)
        {
            DataGridViewComboBoxCell cell = (DataGridViewComboBoxCell) row.Cells[VarDisplayMethodIndex];
            VariableType type = GetRowVarType(row);
            if (cell.Value == null || type != _lastVariableType &&
                !((type == VariableType.Byte && _lastVariableType == VariableType.Word) ||
                  (type == VariableType.Word && _lastVariableType == VariableType.Byte)))
            {
                cell.Items.Clear();
                switch (type)
                {
                    case VariableType.Byte:
                    case VariableType.Word:
                        cell.Items.Add(VariableDisplayMethod.Hexadecimal.ToString());
                        cell.Items.Add(VariableDisplayMethod.Decimal.ToString());
                        cell.Items.Add(VariableDisplayMethod.Binary.ToString());
                        cell.Items.Add(VariableDisplayMethod.Octal.ToString());
                        cell.Value = VariableDisplayMethod.Hexadecimal.ToString();
                        break;
                    case VariableType.Image:
                        cell.Items.Add(VariableDisplayMethod.BlackWhiteImage.ToString());
                        cell.Items.Add(VariableDisplayMethod.GrayscaleImage.ToString());
                        cell.Value = VariableDisplayMethod.BlackWhiteImage.ToString();
                        break;
                    case VariableType.String:
                        cell.Items.Add(VariableDisplayMethod.String.ToString());
                        cell.Value = VariableDisplayMethod.String.ToString();
                        break;
                    default:
                        throw new Exception("Invalid var type");
                }
                _lastVariableType = type;
            }

            return (VariableDisplayMethod) Enum.Parse(typeof(VariableDisplayMethod), cell.Value.ToString());
        }

        private void SetRowValue(DataGridViewRow row, object cellData)
        {
            if (cellData is Image)
            {
                _imageList[row.Index] = (Image) cellData;
                cellData = "Double click for image";
            }
            row.Cells[VarValueIndex].Value = cellData;
        }

        #endregion

        private void UpdateAllRows()
        {
            foreach (DataGridViewRow row in variablesDataView.Rows)
            {
                UpdateRowValue(row);
            }
        }

        private int GetAddressValue(string address)
        {
            IEnumerable<string> tokenList = ExpressionEvaluator.CreateTokenList(address);
            var postFix = ExpressionEvaluator.InfixToPostfix(tokenList);
            int value = _expressionEvaluator.EvalPostfix(postFix);
            return value;
        }

        private void UpdateRowValue(DataGridViewRow row)
        {
            int address;
            try
            {
                address = GetAddressValue(GetRowAddress(row));
            }
            catch (FormatException ex)
            {
                row.Cells[5].Value = string.Format("Error: {0}", ex.Message);
                return;
            }

            string numBytesString = GetRowNumBytes(row);
            VariableType valueTypeEntry = GetRowVarType(row);
            VariableDisplayMethod valueMethod = GetRowVarDisplayMethod(row);

            object value = GetValue(address, numBytesString, valueTypeEntry, valueMethod);
            SetRowValue(row, value);
        }

        private object GetValue(int address, string numBytesString,
            VariableType valueTypeEntry, VariableDisplayMethod varMethod)
        {
            string outputValue;
            int numBytes;
            bool numBytesError = int.TryParse(numBytesString, out numBytes);
            switch (valueTypeEntry)
            {
                case VariableType.Image:
                    return GetVarImage(address, numBytesString, varMethod);
                case VariableType.String:
                    outputValue = GetVarString(address, numBytes);
                    break;
                case VariableType.Word:
                    if (!numBytesError)
                    {
                        return "Error: missing number of bytes";
                    }
                    outputValue = GetVarWord(address, varMethod, numBytes);

                    break;
                default:
                    if (!numBytesError)
                    {
                        return "Error: missing number of bytes";
                    }
                    outputValue = GetVarByte(address, varMethod, numBytes);
                    break;
            }

            return outputValue;
        }

        private string GetVarByte(int address, VariableDisplayMethod varMethod, int numBytes)
        {
            string outputValue = string.Empty;
            int convertMethod = GetConvertBase(varMethod);

            for (int i = 0; i < numBytes; i++)
            {
                byte baseValue = _debugger.ReadByte((ushort) (address + i));
                switch (varMethod)
                {
                    case VariableDisplayMethod.Decimal:
                        outputValue += Convert.ToString(baseValue, convertMethod);
                        break;
                    case VariableDisplayMethod.Binary:
                        outputValue += Convert.ToString(baseValue, convertMethod).PadLeft(16, '0');
                        break;
                    case VariableDisplayMethod.Octal:
                        outputValue += Convert.ToString(baseValue, convertMethod).PadLeft(8, '0');
                        break;
                    default:
                        outputValue += Convert.ToString(baseValue, convertMethod).PadLeft(4, '0');
                        break;
                }
                outputValue += " ";
            }
            return outputValue;
        }

        private string GetVarWord(int address, VariableDisplayMethod varMethod, int numBytes)
        {
            string outputValue = string.Empty;
            int convertMethod = GetConvertBase(varMethod);

            for (int i = 0; i < numBytes * 2; i += 2)
            {
                ushort baseValue = _debugger.ReadShort((ushort) (address + i));
                switch (varMethod)
                {
                    case VariableDisplayMethod.Decimal:
                        outputValue += Convert.ToString(baseValue, convertMethod);
                        break;
                    case VariableDisplayMethod.Binary:
                        outputValue += Convert.ToString(baseValue, convertMethod).PadLeft(16, '0');
                        break;
                    case VariableDisplayMethod.Octal:
                        outputValue += Convert.ToString(baseValue, convertMethod).PadLeft(8, '0');
                        break;
                    default:
                        outputValue += Convert.ToString(baseValue, convertMethod).PadLeft(4, '0');
                        break;
                }
                outputValue += " ";
            }
            return outputValue;
        }

        private static int GetConvertBase(VariableDisplayMethod varMethod)
        {
            int convertMethod;
            switch (varMethod)
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
            return convertMethod;
        }

        private string GetVarString(int address, int numBytes)
        {
            string outputValue = string.Empty;
            if (numBytes == -1)
            {
                int i = 0;
                char charToAdd;
                do
                {
                    charToAdd = (char) _debugger.ReadByte((ushort) (address + i));
                    outputValue += charToAdd.ToString();
                    i++;
                } while (charToAdd != '\0');
            }
            else
            {
                for (int i = 0; i < numBytes; i++)
                {
                    outputValue += ((char) _debugger.ReadByte((ushort) (address + i))).ToString();
                }
            }

            return outputValue;
        }

        private object GetVarImage(int address, string numBytesString, VariableDisplayMethod varMethod)
        {
            bool isGrayscale = varMethod == VariableDisplayMethod.GrayscaleImage;
            Match match = Regex.Match(numBytesString, @"(?<width>\d+)\s*x\s*(?<height>\d+)");
            int width = int.Parse(match.Groups["width"].Value);
            int height = int.Parse(match.Groups["height"].Value);

            Bitmap screen = new Bitmap(width, height);
            int row = 0, col = 0, grayscale = (width / 8) * height;
            if (width % 8 != 0)
            {
                return "Width is not a multiple of 8!";
            }

            for (int i = 0; i < width / 8 * height; i++)
            {
                int anotherbyte = 0;
                int abyte = _debugger.ReadByte((ushort) (address + i));
                if (isGrayscale)
                {
                    anotherbyte = _debugger.ReadByte((ushort) (address + grayscale + i));
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
        }

        private void temp_FormClosed(object sender, FormClosedEventArgs e)
        {
            foreach (BitmapViewer test in _openBitmaps.Where(test => test.Tag == ((Form) sender).Tag))
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

            BitmapViewer temp = new BitmapViewer(_imageList[e.RowIndex])
            {
                Tag = e.RowIndex
            };
            temp.Show();
            _openBitmaps.Add(temp);
            temp.FormClosed += temp_FormClosed;
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

        private void variablesDataView_RowPostPaint(object sender, DataGridViewRowPostPaintEventArgs e)
        {
            variablesDataView.Rows[e.RowIndex].DefaultCellStyle.ForeColor = Enabled ? Color.Black : Color.Gray;
        }
    }
}