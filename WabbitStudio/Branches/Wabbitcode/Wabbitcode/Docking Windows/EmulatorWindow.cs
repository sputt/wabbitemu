namespace Revsoft.Wabbitcode.Docking_Windows
{
    using System;
    using System.Drawing;
    using System.Runtime.InteropServices;
    using System.Threading;
    using System.Windows.Forms;

    using Revsoft.Docking;
    using Revsoft.Wabbitcode.Classes;

    public partial class EmulatorWindow : ToolWindow
    {
        private const int VK_LSHIFT = 0xA0;
        private const int VK_RSHIFT = 0xA1;
        private const int VK_SHIFT = 16;
        private const int WM_KEYDOWN = 0x100;
        private const int WM_KEYUP = 0x101;
        private const int WM_SYSKEYDOWN = 0x104;
        private const int WM_SYSKEYUP = 0x105;

        private Image screen = new Bitmap(96, 64);

        public EmulatorWindow()
        {
            InitializeComponent();
            this.emulator = new Emulator(GlobalClass.GetRomFile());

            // GlobalClass.emulator = emulator;
            // emulator.SetLCD(screenBox.Handle);
            this.emulator.DrawScreen(screenBox.Handle);
            updateTimer.Start();
        }

        public Revsoft.Wabbitcode.Emulator emulator
        {
            get;
            set;
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (msg.Msg == WM_KEYDOWN)
            {
                if ((int)msg.WParam == VK_SHIFT)
                {
                    if ((GetKeyState(VK_LSHIFT) & 0xFF00) != 0)
                    {
                        msg.WParam = (IntPtr)VK_LSHIFT;
                    }
                    else
                    {
                        msg.WParam = (IntPtr)VK_RSHIFT;
                    }
                }

                this.emulator.SendKeypress((int)msg.WParam);
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
                        this.emulator.ReleaseKeypress((int)msg.WParam);
                    }

                    if ((GetKeyState(VK_RSHIFT) & 0xFF00) == 0)
                    {
                        msg.WParam = (IntPtr)VK_RSHIFT;
                    }
                }

                this.emulator.ReleaseKeypress((int)msg.WParam);
            }

            return base.ProcessKeyEventArgs(ref msg);
        }

        [DllImport("user32.dll")]
        private static extern short GetKeyState(int key);

        private void backgroundWorker1_DoWork(object sender, System.ComponentModel.DoWorkEventArgs e)
        {
            screen = emulator.DrawScreen();
            if (doubleBox.Checked)
            {
                screen = GlobalClass.resizeImage(screen, 256, 128);
            }
        }

        private void updateTimer_Tick(object sender, EventArgs e)
        {
            if (!backgroundWorker1.IsBusy)
            {
                backgroundWorker1.RunWorkerAsync();
                screenBox.Image = screen;
            }

            // Thread.Sleep(100);
        }
    }
}