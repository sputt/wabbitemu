using System.Drawing;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode
{
    public partial class BitmapViewer : Form
    {
        public BitmapViewer(Image image)
        {
            InitializeComponent();
            Size = new Size(image.Size.Width + 42 + vertRuler.Width, image.Size.Height + 62 + horzRuler.Height);
            horzRuler.Location = new Point(33, 12);
            vertRuler.Location = new Point(12, 30);
            horzRuler.Width = image.Size.Width;
            vertRuler.Height = image.Size.Height;
            pictureBox.Image = image;
            pictureBox.Size = image.Size;
            pictureBox.BackColor = Color.White;
            pictureBox.Location = new Point(18 + vertRuler.Width, 18 + horzRuler.Height);
            button1.Location = new Point(Width/2 - button1.Width/2, Height - (button1.Height + button1.Height/2)) ;
            drawLines();
        }

        private void drawLines()
        {
            Bitmap image = new Bitmap(horzRuler.Width, horzRuler.Height);
            horzRuler.Image = image;
            int horz = 0;
            while (horz < horzRuler.Width)
            {
                Graphics g = Graphics.FromImage(horzRuler.Image);
                Pen p = new Pen(Color.Black);
                g.DrawLine(p, horz, 0, horz, horzRuler.Height);
                horz += 16;
				p.Dispose();
            }
            int vert = 0;
            image = new Bitmap(vertRuler.Width, vertRuler.Height);
            vertRuler.Image = image;
            while (vert < vertRuler.Height)
            {
                Graphics g = Graphics.FromImage(vertRuler.Image);
                Pen p = new Pen(Color.Black);
                g.DrawLine(p, 0, vert, vertRuler.Width, vert);
                vert += 16;
				p.Dispose();
            }

        }

        private void button1_Click(object sender, System.EventArgs e)
        {
            Close();
        }
    }
}
