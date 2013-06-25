//using System;
//using System.Drawing;
//using System.Windows.Forms;
//using WabbitemuLib;
//using System.Runtime.InteropServices;
//using System.Drawing.Drawing2D;
//using Revsoft.Wabbitcode.Classes;

//namespace Revsoft.Wabbitcode
//{
//    public partial class Form1 : Form
//    {
//        Wabbitemu calc;
//        Bitmap calcBitmap;
//        IntPtr scan0 = Marshal.AllocHGlobal(128 * 64);
//        public Form1()
//        {
//            InitializeComponent();
//            Timer timer = new Timer();
//            timer.Interval = 100;
//            timer.Tick += new EventHandler(timer_Tick);
//            calc = new Wabbitemu();
//            OpenFileDialog dialog = new OpenFileDialog()
//            {
//                Filter = "ROMS (*.rom)|*.rom",
//                Title = "Open ROM file"
//            };
//            if (dialog.ShowDialog() != DialogResult.OK)
//                return;

//            calc.LoadFile(dialog.FileName);
//            calc.Running = true;
//            pictureBox1.Width = 256;
//            pictureBox1.Height = 128;
			
//            calcBitmap = new Bitmap(128, 64, 128, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, scan0);
//            var palette = calcBitmap.Palette;
//            for (int i = 0; i < 255; i++)
//            {
//                palette.Entries[i] = Color.FromArgb(0x9e * (256 - i) / 255, (0xAB * (256 - i)) / 255, (0x88 * (256 - i)) / 255);
//            }
//            calcBitmap.Palette = palette;
//            pictureBox1.Image = calcBitmap;
//            timer.Start();

//            calc.Keypad.PressVirtKey((int) Keys.F12);
//            calc.Keypad.ReleaseVirtKey((int) Keys.F12);
//        }

//        void timer_Tick(object sender, EventArgs e)
//        {
//            Marshal.Copy((byte []) calc.LCD.Display, 0, scan0, 128 * 64);
//            pictureBox1.Refresh();
//        }

//        IntPtr last_shift;

//        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
//        {
			
//            return base.ProcessCmdKey(ref msg, keyData);
//        }

//        protected override bool ProcessKeyEventArgs(ref Message msg)
//        {
//            switch (msg.Msg)
//            {
//                case NativeMethods.WM_KEYDOWN:
//                    if ((int)msg.WParam == NativeMethods.VK_SHIFT)
//                    {
//                        if ((NativeMethods.GetKeyState(NativeMethods.VK_LSHIFT) & 0xFF00) != 0)
//                            msg.WParam = last_shift = (IntPtr)NativeMethods.VK_LSHIFT;
//                        else
//                            msg.WParam = last_shift = (IntPtr)NativeMethods.VK_RSHIFT;
//                    }
//                    break;
//                case NativeMethods.WM_KEYUP:
//                    if ((int)msg.WParam == NativeMethods.VK_SHIFT)
//                    {
//                        msg.WParam = last_shift;
//                    }
//                    break;
//            }
//            return base.ProcessKeyEventArgs(ref msg);
//        }

//        private void Form1_KeyDown(object sender, KeyEventArgs e)
//        {

//            calc.Keypad.PressVirtKey(e.KeyValue);
//        }

//        private void Form1_KeyUp(object sender, KeyEventArgs e)
//        {
//            calc.Keypad.ReleaseVirtKey(e.KeyValue);
//        }

//        private void pictureBox1_Paint(object sender, PaintEventArgs e)
//        {
//            e.Graphics.InterpolationMode = InterpolationMode.NearestNeighbor;
//            e.Graphics.DrawImage(calcBitmap, new Rectangle(0, 0, 192, 128), 0, 0, 96, 64, GraphicsUnit.Pixel);
//        }
//    }
//}
