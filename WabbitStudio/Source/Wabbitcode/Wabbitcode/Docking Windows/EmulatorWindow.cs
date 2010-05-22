using System;
using Revsoft.Docking;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Drawing;
using Revsoft.Wabbitcode.Classes;
using System.Threading;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class EmulatorWindow : ToolWindow
    {
        public EmulatorWindow()
        {
            InitializeComponent();
            emulator = new Emulator(GlobalClass.GetRomFile());
            //GlobalClass.emulator = emulator;
            //emulator.SetLCD(screenBox.Handle);
            emulator.DrawScreen(screenBox.Handle);
            updateTimer.Start();
        }

        public Revsoft.Wabbitcode.Emulator emulator { get; set; }

        private const int WM_KEYDOWN = 0x100;
        private const int WM_SYSKEYDOWN = 0x104;
        private const int WM_KEYUP = 0x101;
        private const int WM_SYSKEYUP = 0x105;
        private const int VK_SHIFT = 16;
        private const int VK_LSHIFT = 0xA0;
        private const int VK_RSHIFT = 0xA1;
        [DllImport("user32.dll")]
        private static extern short GetKeyState(int key);

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (msg.Msg == WM_KEYDOWN)
            {
                if ((int)msg.WParam == VK_SHIFT)
                {
                    if ((GetKeyState(VK_LSHIFT) & 0xFF00) != 0)
                        msg.WParam = (IntPtr)VK_LSHIFT;
                    else
                        msg.WParam = (IntPtr)VK_RSHIFT;
                }
                emulator.SendKeypress((int)msg.WParam);
            }
            return base.ProcessCmdKey(ref msg, keyData);
        }

        protected override bool ProcessKeyEventArgs(ref Message msg)
        {
            if (msg.Msg == WM_KEYUP)
            {
                if ((int)msg.WParam == VK_SHIFT)
                {
                    if ((GetKeyState(VK_LSHIFT) & 0xFF00) == 0)
                    {
                        msg.WParam = (IntPtr)VK_LSHIFT;
                        emulator.ReleaseKeypress((int)msg.WParam);
                    }
                    if ((GetKeyState(VK_RSHIFT) & 0xFF00) == 0)
                        msg.WParam = (IntPtr)VK_RSHIFT;
                }
                emulator.ReleaseKeypress((int)msg.WParam);
            }
            return base.ProcessKeyEventArgs(ref msg);
        }

        Image screen = new Bitmap(96, 64);
        private void updateTimer_Tick(object sender, EventArgs e)
        {
            if (!backgroundWorker1.IsBusy)
            {
                backgroundWorker1.RunWorkerAsync();
                screenBox.Image = screen;
            }
            //Thread.Sleep(100);
        }

        private void backgroundWorker1_DoWork(object sender, System.ComponentModel.DoWorkEventArgs e)
        {
            screen = emulator.DrawScreen();
            if (doubleBox.Checked)
                screen = GlobalClass.resizeImage(screen, 256, 128);
        }
    }
}