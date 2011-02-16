using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;

namespace Revsoft.Wabbitcode
{
    public class Pixel
	{
		public int row;
		public int col;
		public Color color = Color.Black;
        public bool masked;
		public Pixel(int row, int col)
		{
			this.row = row;
			this.col = col;   
		}
		public Pixel(int row, int col, Color color)
		{
			this.row = row;
			this.col = col;
			this.color = color;
		}
		public Rectangle GetRectangle(int pixelSize)
		{
            Rectangle pixel = new Rectangle(((pixelSize + 1) * col) + 1, ((pixelSize + 1) * row) + 1,
				((pixelSize + 1) * (col + 1)) + 1, ((pixelSize + 1) * (row + 1)) + 1);
            return pixel;
		}
	}
    public class Sprite
    {
        public int width;
        public int height;
        public int pixelSize = 8;
        public int shades = 2;
        public bool masked = false;
        public Pixel[,] pixelArray;
        public Sprite(int width, int height)
        {
            this.width = width;
            this.height = height;
            pixelArray = new Pixel[width, height];
        }
        public void DrawSprite(Bitmap image)
        {
            Graphics g = Graphics.FromImage(image);
            foreach (Pixel pixel in pixelArray)
            {
                Rectangle rect = pixel.GetRectangle(pixelSize);

            }
        }
    }
}
