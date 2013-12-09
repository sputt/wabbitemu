using System.Drawing;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.GUI.Dialogs
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
            button1.Location = new Point(Width/2 - button1.Width/2, Height - (button1.Height + button1.Height/2));
            DrawGridLines();
        }

        private void button1_Click(object sender, System.EventArgs e)
        {
            Close();
        }

        private void DrawGridLines()
        {
            Bitmap image = new Bitmap(horzRuler.Width, horzRuler.Height);
            horzRuler.Image = image;
            int horz = 0;
            using (Graphics g = Graphics.FromImage(horzRuler.Image))
            {
                using (Pen p = new Pen(Color.Black))
                {
                    while (horz < horzRuler.Width)
                    {
                        g.DrawLine(p, horz, 0, horz, horzRuler.Height);
                        horz += 16;
                    }
                }
            }

            int vert = 0;
            image = new Bitmap(vertRuler.Width, vertRuler.Height);
            vertRuler.Image = image;
            using (Graphics g = Graphics.FromImage(vertRuler.Image))
            {
                using (Pen p = new Pen(Color.Black))
                {
                    while (vert < vertRuler.Height)
                    {
                        g.DrawLine(p, 0, vert, vertRuler.Width, vert);
                        vert += 16;
                    }
                }
            }
        }
    }
}