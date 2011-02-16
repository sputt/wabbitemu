using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode
{
    public partial class spriteEditor : Form
    {
        public spriteEditor()
        {
            InitializeComponent();
            Bitmap bm = new Bitmap(96 * (enlargedPixelSize + 1) + 1, 64 * (enlargedPixelSize + 1) + 1);
            pixelBox.Image = bm;
            initializeBox();
        }
        
        private Color pixelColor = Color.Black;
        int col = 0;
        int row = 0;
        int enlargedPixelSize = 16;
        private void initializeBox()
        {
            int width = (int)widthInput.Value;
            int height = (int)heightInput.Value;
            if ((width > 24 || height > 24) && enlargedPixelSize != 8)
            {
                enlargedPixelSize = 8;
                pixelBox.Image = resizeImage((Bitmap)pixelBox.Image, (enlargedPixelSize + 1) * width + 1, (enlargedPixelSize + 1) * height + 1);
            }

            else if (width <= 24 && height <= 24 && enlargedPixelSize != 16)
            {
                enlargedPixelSize = 16;
                pixelBox.Image = resizeImage((Bitmap)pixelBox.Image, width * enlargedPixelSize, height * enlargedPixelSize);
            }
            pixelBox.Size = new Size((enlargedPixelSize + 1) * width + 1, (enlargedPixelSize + 1) * height + 1);
            this.Size = new Size((enlargedPixelSize + 1) * width + 211, (enlargedPixelSize + 1) * height + 91);
            drawGridLines();
        }

        private Bitmap resizeImage(Bitmap img, int width, int height)
        {
            Bitmap resized = new Bitmap(width + 1, height + 1);
            Graphics g = Graphics.FromImage((Image)resized);
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
            g.DrawImage(img, 0, 0, width + 1, height + 1);
            g.Dispose();

            return (Bitmap)resized;
        }

        private void drawGridLines()
        {
            Graphics g = Graphics.FromImage(pixelBox.Image);
            Pen pen = new Pen(Color.FromArgb(0, 0, 1));
            for (int x = 0; x < pixelBox.Width; x += enlargedPixelSize + 1)
            {
                g.DrawLine(pen, x, 0, x, (enlargedPixelSize + 1) * (int)heightInput.Value + 1);
            }
            for (int y = 0; y < pixelBox.Height; y += enlargedPixelSize + 1)
            {
                g.DrawLine(pen, 0, y, (enlargedPixelSize + 1) * (int)widthInput.Value + 1, y);
            }
        }

        private void widthInput_ValueChanged(object sender, EventArgs e)
        {
            initializeBox();
            pixelBox.Invalidate();
        }

        private void heightInput_ValueChanged(object sender, EventArgs e)
        {
            initializeBox();
            pixelBox.Invalidate();
        }
        private void clearBorders()
        {
            colorBlack.BorderStyle = BorderStyle.None;
            colorDarkGray.BorderStyle = BorderStyle.None;
            colorLightGray.BorderStyle = BorderStyle.None;
            colorWhite.BorderStyle = BorderStyle.None;
        }

        private void colorWhite_Click(object sender, EventArgs e)
        {
            clearBorders();
            colorWhite.BorderStyle = BorderStyle.Fixed3D;
            pixelColor = Color.White;
        }

        private void colorLightGray_Click(object sender, EventArgs e)
        {
            clearBorders();
            colorLightGray.BorderStyle = BorderStyle.Fixed3D;
            pixelColor = Color.DarkGray;
        }

        private void colorDarkGray_Click(object sender, EventArgs e)
        {
            clearBorders();
            colorDarkGray.BorderStyle = BorderStyle.Fixed3D;
            pixelColor = Color.DimGray;
        }

        private void colorBlack_Click(object sender, EventArgs e)
        {
            clearBorders();
            colorBlack.BorderStyle = BorderStyle.Fixed3D;
            pixelColor = Color.Black;
        }

        private void grayscaleCheck_CheckedChanged(object sender, EventArgs e)
        {
            if (grayscaleCheck.Checked == false)
            {
                colorLightGray.Hide();
                colorDarkGray.Hide();
                colorBlack.Location = new Point(colorBlack.Location.X - colorBlack.Width, colorBlack.Location.Y);
                colorBlack.Width *= 2;
                colorWhite.Width *= 2;
            }
            else
            {
                colorLightGray.Show();
                colorDarkGray.Show();
                colorBlack.Width /= 2;
                colorBlack.Location = new Point(colorBlack.Location.X + colorBlack.Width, colorBlack.Location.Y);
                colorWhite.Width /= 2;
            }
        }
        bool mouseDown = false;
        private void pixelBox_MouseDown(object sender, MouseEventArgs e)
        {
            //int curX = this.PointToClient(e.Location).X - pixelBox.Location.X;
            //int curY = this.PointToClient(e.Location).Y - pixelBox.Location.Y;
            mouseDown = true;
            fillSquare(e);
        }

        private void pixelBox_MouseMove(object sender, MouseEventArgs e)
        {
            if (mouseDown)
            {
                fillSquare(e);
            }

        }

        private void pixelBox_MouseUp(object sender, MouseEventArgs e)
        {
            mouseDown = false;
        }
        private void fillSquare(MouseEventArgs e)
        {
            int curX = e.Location.X;
            int curY = e.Location.Y;
            col = (curX - 1) / (enlargedPixelSize + 1);
            row = (curY - 1) / (enlargedPixelSize + 1);
            if (e.Button == MouseButtons.Left)
            {
                Graphics g = Graphics.FromImage(pixelBox.Image);
                Brush rect = new SolidBrush(pixelColor);
                g.FillRectangle(rect, (enlargedPixelSize + 1) * col + 1, (enlargedPixelSize + 1) * row + 1, enlargedPixelSize, enlargedPixelSize);
                pixelBox.Refresh();
            }
            else
            {
                Graphics g = Graphics.FromImage(pixelBox.Image);
                Brush rect = new SolidBrush(Color.White);
                g.FillRectangle(rect, (enlargedPixelSize + 1) * col + 1, (enlargedPixelSize + 1) * row + 1, enlargedPixelSize, enlargedPixelSize);
                pixelBox.Refresh();
            }
        }

        private void clearButton_Click(object sender, EventArgs e)
        {
            DialogResult clear = MessageBox.Show("Are you sure you want to clear this image?", "Clear Image", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning);
            if (clear == DialogResult.OK)
            {
                pixelBox.Image.Dispose();
                Bitmap bm = new Bitmap(96 * (enlargedPixelSize + 1) + 1, 64 * (enlargedPixelSize + 1) + 1);
                pixelBox.Image = bm;
                initializeBox();
            }
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            removeLines();
            makeRealBitmap();
            this.Hide();
        }

        private void removeLines()
        {
            Graphics g = Graphics.FromImage(pixelBox.Image);
            Pen pen = new Pen(Color.White);
            for (int x = 0; x < pixelBox.Width; x += enlargedPixelSize + 1)
            {
                g.DrawLine(pen, x, 0, x, (enlargedPixelSize + 1) * (int)heightInput.Value + 1);
            }
            for (int y = 0; y < pixelBox.Height; y += enlargedPixelSize + 1)
            {
                g.DrawLine(pen, 0, y, (enlargedPixelSize + 1) * (int)widthInput.Value + 1, y);
            }
        }
        private void makeRealBitmap()
        {
            getColorArray();
            int counter = 0;
            Color pixColor;
            Bitmap finalSprite = new Bitmap((int)widthInput.Value, (int)heightInput.Value);
            Graphics gr = Graphics.FromImage(finalSprite);
            for (int x = 0; x < widthInput.Value; x++)
            {
                for (int y = 0; y < heightInput.Value; y++)
                {
                    pixColor = colorsRead[counter];
                    finalSprite.SetPixel(x, y, pixColor);
                    counter++;
                }
            }
            pixelBox.Image = finalSprite;
            pixelBox.Width = (int)widthInput.Value;
            pixelBox.Height = (int)heightInput.Value;
        }
        List<Color> colorsRead = new List<Color>();
        private void getColorArray()
        {
            Color rectColor;
            Bitmap drawing = (Bitmap)pixelBox.Image;
            for (int x = 3; x < (enlargedPixelSize + 1) * widthInput.Value + 1; x += enlargedPixelSize + 1)
            {
                for (int y = 3; y < (enlargedPixelSize + 1) * widthInput.Value + 1; y += enlargedPixelSize + 1)
                {
                    rectColor = drawing.GetPixel(x, y);
                    colorsRead.Add(rectColor);
                }
            }
        }
    }
}
