using System;
using System.Drawing;
using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    internal abstract class ImageVariableDisplayController : AbstractVariableDisplayController<Size, Image>
    {
        protected Bitmap Screen { get; private set; }
        protected Size ImageSize { get; private set; }

        public override int Size { get { return 1; } }

        protected ImageVariableDisplayController(ExpressionEvaluator evaluator) : base(evaluator)
        {
        }

        protected override Size ParseValidSize(string sizeString)
        {
            Match match = Regex.Match(sizeString, @"(?<width>\d+)\s*x\s*(?<height>\d+)");
            if (!match.Success)
            {
                throw new ArgumentException("Invalid size string");
            }

            int imageWidth = int.Parse(match.Groups["width"].Value);
            int imageHeight = int.Parse(match.Groups["height"].Value);

            if ((imageWidth % 8) != 0)
            {
                throw new ArgumentException("Width is not a multiple of 8!");
            }

            return new Size(imageWidth, imageHeight);
        }

        protected override string GetDisplayValue(IWabbitcodeDebugger debugger, int address, Size size)
        {
            return "Double click for image";
        }

        public override Image GetActualValue(IWabbitcodeDebugger debugger, int address, Size size)
        {
            return GetVarImage(debugger, address, size);
        }

        protected abstract byte[] ReadBytes(IWabbitcodeDebugger debugger, int address);
        protected abstract void SetPixel(byte[] bytes, int bit, int row, int col);

        private Image GetVarImage(IWabbitcodeDebugger debugger, int address, Size size)
        {
            ImageSize = size;
            Screen = new Bitmap(size.Width, size.Height);
            int row = 0, col = 0;

            for (int i = 0; i < size.Width / 8 * size.Height; i++)
            {
                var bytes = ReadBytes(debugger, address + i);
                
                for (int bit = 128; bit > 0; bit /= 2)
                {
                    SetPixel(bytes, bit, row, col);
                    col++;
                }

                if (col < size.Width)
                {
                    continue;
                }

                col = 0;
                row++;
            }

            return Screen.ResizeImage(size.Width * 2, size.Height * 2);
        }
    }

    class BlackAndWhiteImageVariableDisplayController : ImageVariableDisplayController
    {
        public BlackAndWhiteImageVariableDisplayController(ExpressionEvaluator evaluator) : base(evaluator)
        {
        }

        public override string Name
        {
            get { return "Black and White Image"; }
        }

        protected override byte[] ReadBytes(IWabbitcodeDebugger debugger, int address)
        {
            return new[] { debugger.ReadByte((ushort) address)};
        }

        protected override void SetPixel(byte[] bytes, int bit, int row, int col)
        {
            if ((bytes[0] & bit) != 0)
            {
                Screen.SetPixel(col, row, Color.Black);
            }
        }
    }

    class GrayImageVariableDisplayController : ImageVariableDisplayController
    {
        public GrayImageVariableDisplayController(ExpressionEvaluator evaluator) : base(evaluator)
        {
        }

        public override string Name
        {
            get { return "Gray Image"; }
        }


        protected override byte[] ReadBytes(IWabbitcodeDebugger debugger, int address)
        {
            var bytes = new byte[2];
            int grayscaleOffset = (ImageSize.Width / 8) * ImageSize.Height;
            bytes[0] = debugger.ReadByte((ushort) address);
            bytes[1] = debugger.ReadByte((ushort) (address + grayscaleOffset));
            return bytes;
        }

        protected override void SetPixel(byte[] bytes, int bit, int row, int col)
        {
            var leftByte = bytes[0];
            var rightByte = bytes[1];

            if ((leftByte & bit) != 0 && (rightByte & bit) != 0)
            {
                Screen.SetPixel(col, row, Color.Black);
            }
            else if ((leftByte & bit) != 0 && (rightByte & bit) == 0)
            {
                Screen.SetPixel(col, row, Color.DarkGray);
            }
            else if ((leftByte & bit) == 0 && (rightByte & bit) != 0)
            {
                Screen.SetPixel(col, row, Color.LightGray);
            }
        }
    }
}