using System;
using System.Drawing;
using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows
{
    public class TrackingVariableRowModel : IDisposable
    {
        private const int InvalidValue = -2;
        private readonly ExpressionEvaluator _expressionEvaluator;
        private readonly IDebuggerService _debuggerService;

        private int _numBytes = InvalidValue;
        private int _imageWidth = InvalidValue;
        private int _imageHeight = InvalidValue;
        private IWabbitcodeDebugger _debugger;
        private bool _disposed;
        private string _address;
        private VariableType _valueType;
        private VariableDisplayMethod _displayMethod;
        private string _cachedValue;

        #region Properties

        [UsedImplicitly]
        public string Address
        {
            get { return _address; }
            set
            {
                IsCachedValueValid = false;
                _address = value;
            }
        }

        [UsedImplicitly]
        public string NumBytesString
        {
            get
            {
                if (_numBytes != InvalidValue)
                {
                    return _numBytes.ToString();
                }

                if (_imageWidth != InvalidValue && _imageHeight != InvalidValue)
                {
                    return _imageWidth + "x" + _imageHeight;
                }

                return "";
            }
            set
            {
                Match match = Regex.Match(value, @"(?<width>\d+)\s*x\s*(?<height>\d+)");
                if (match.Success)
                {
                    _numBytes = InvalidValue;

                    IsCachedValueValid = false;
                    _imageWidth = int.Parse(match.Groups["width"].Value);
                    _imageHeight = int.Parse(match.Groups["height"].Value);
                    if ((_imageWidth%8) != 0)
                    {
                        throw new ArgumentException("Width is not a multiple of 8!");
                    }
                }
                else
                {
                    _imageWidth = InvalidValue;
                    _imageHeight = InvalidValue;

                    if (int.TryParse(value, out _numBytes))
                    {
                        IsCachedValueValid = false;
                        return;
                    }

                    _numBytes = InvalidValue;
                    throw new ArgumentException("Invalid number");
                }
            }
        }

        [UsedImplicitly]
        public VariableType ValueType
        {
            get { return _valueType; }
            set
            {
                IsCachedValueValid = true;
                _valueType = value;
            }
        }

        [UsedImplicitly]
        public VariableDisplayMethod DisplayMethod
        {
            get { return _displayMethod; }
            set
            {
                IsCachedValueValid = false;
                _displayMethod = value;
            }
        }

        [UsedImplicitly]
        public string Value
        {
            get {
                if (IsCachedValueValid)
                {
                    return _cachedValue;
                }
                _cachedValue = GetCachedValue();
                IsCachedValueValid = true;
                return _cachedValue;
            }
        }

        private string GetCachedValue()
        {
            int address;
            try
            {
                address = GetAddressValue(Address);
            }
            catch (FormatException ex)
            {
                return string.Format("Error: {0}", ex.Message);
            }

            ValueImage = null;
            switch (ValueType)
            {
                case VariableType.Image:
                    ValueImage = GetVarImage(address);
                    return "Double click for image";
                case VariableType.String:
                    return GetVarString(address);
                case VariableType.Word:
                    return GetVarWord(address);
                default:
                    return GetVarByte(address);
            }
        }

        public Image ValueImage { get; private set; }

        public  bool IsCachedValueValid { get; set; }

        #endregion

        public TrackingVariableRowModel(IDebuggerService debuggerService, ExpressionEvaluator expressionEvaluator)
        {
            _debuggerService = debuggerService;
            _debugger = _debuggerService.CurrentDebugger;
            _expressionEvaluator = expressionEvaluator;
            _valueType = VariableType.Byte;
            _displayMethod = VariableDisplayMethod.Hexadecimal;

            _debuggerService.OnDebuggingStarted += DebuggerService_OnDebuggingStarted;
            _debuggerService.OnDebuggingEnded += DebuggerService_OnDebuggingEnded;
        }

        ~TrackingVariableRowModel()
        {
           Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
        }

        private void Dispose(bool disposing)
        {
            if (_disposed)
            {
                return;
            }

            if (disposing)
            {
                _debuggerService.OnDebuggingStarted -= DebuggerService_OnDebuggingStarted;
                _debuggerService.OnDebuggingEnded -= DebuggerService_OnDebuggingEnded;
            }

            _disposed = true;
        }

        private void DebuggerService_OnDebuggingStarted(object sender, DebuggingEventArgs e)
        {
            _debugger = e.Debugger;
        }

        private void DebuggerService_OnDebuggingEnded(object sender, DebuggingEventArgs e)
        {
            _debugger = null;
        }

        private int GetAddressValue(string address)
        {
            var tokenList = ExpressionEvaluator.CreateTokenList(address);
            var postFix = ExpressionEvaluator.InfixToPostfix(tokenList);
            int value = _expressionEvaluator.EvalPostfix(postFix);
            return value;
        }

        private string GetVarByte(int address)
        {
            return GetVarValue(address, sizeof(byte));
        }

        private string GetVarWord(int address)
        {
            return GetVarValue(address, sizeof(ushort));
        }

        private string GetVarValue(int address, int scale)
        {
            if (_numBytes == InvalidValue)
            {
                return "Invalid number of bytes";
            }

            string outputValue = string.Empty;
            int convertMethod = DisplayMethodToBase(DisplayMethod);

            for (int i = 0; i < _numBytes * scale; i++)
            {
                byte baseValue = _debugger.ReadByte((ushort)(address + i));
                switch (DisplayMethod)
                {
                    case VariableDisplayMethod.Decimal:
                        outputValue += Convert.ToString(baseValue, convertMethod);
                        break;
                    case VariableDisplayMethod.Binary:
                        outputValue += Convert.ToString(baseValue, convertMethod).PadLeft(8 * scale, '0');
                        break;
                    case VariableDisplayMethod.Octal:
                        outputValue += Convert.ToString(baseValue, convertMethod).PadLeft(4 * scale, '0');
                        break;
                    default:
                        outputValue += Convert.ToString(baseValue, convertMethod).PadLeft(2 * scale, '0');
                        break;
                }
                outputValue += " ";
            }
            return outputValue;
        }

        private string GetVarString(int address)
        {
            string outputValue = string.Empty;
            if (_numBytes == -1)
            {
                int i = 0;
                char charToAdd;
                do
                {
                    charToAdd = (char)_debugger.ReadByte((ushort)(address + i));
                    outputValue += charToAdd.ToString();
                    i++;
                } while (charToAdd != '\0');
            }
            else
            {
                for (int i = 0; i < _numBytes; i++)
                {
                    outputValue += ((char)_debugger.ReadByte((ushort)(address + i))).ToString();
                }
            }

            return outputValue;
        }

        private Image GetVarImage(int address)
        {
            bool isGrayscale = DisplayMethod == VariableDisplayMethod.GrayscaleImage;

            Bitmap screen = new Bitmap(_imageWidth, _imageHeight);
            int row = 0, col = 0, grayscale = (_imageWidth / 8) * _imageHeight;

            for (int i = 0; i < _imageWidth / 8 * _imageHeight; i++)
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

                if (col < _imageWidth)
                {
                    continue;
                }
                col = 0;
                row++;
            }

            return screen.ResizeImage(_imageWidth * 2, _imageHeight * 2);
        }

        private static int DisplayMethodToBase(VariableDisplayMethod displayMethod)
        {
            int convertMethod;
            switch (displayMethod)
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
    }
}