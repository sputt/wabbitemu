using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;

using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Debugger;

using WabbitemuLib;

using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode.Docking_Windows
{
    public partial class DebugPanel : ToolWindow
    {
        private IntPtr scan0 = Marshal.AllocHGlobal(128 * 64);
        private bool updating;
        private MainFormRedone mainForm;
        private WabbitcodeDebugger debugger;

        public DebugPanel(MainFormRedone mainForm)
        {
            InitializeComponent();
            
            this.mainForm = mainForm;
            afBox.ContextMenu = contextMenu1;
            afpBox.ContextMenu = contextMenu1;
            bcBox.ContextMenu = contextMenu1;
            bcpBox.ContextMenu = contextMenu1;
            deBox.ContextMenu = contextMenu1;
            depBox.ContextMenu = contextMenu1;
            hlBox.ContextMenu = contextMenu1;
            hlpBox.ContextMenu = contextMenu1;
            ixBox.ContextMenu = contextMenu1;
            iyBox.ContextMenu = contextMenu1;
            pcBox.ContextMenu = contextMenu1;
            spBox.ContextMenu = contextMenu1;
        }

        public override void Copy()
        {
            if (ActiveControl is TextBox)
            {
                ((TextBox)ActiveControl).Copy();
            }
            else if (ActiveControl is PictureBox)
            {
                Clipboard.SetImage(((PictureBox)ActiveControl).Image);
            }
        }

        public override void Cut()
        {
            if (ActiveControl is TextBox)
            {
                ((TextBox)ActiveControl).Cut();
            }
            else if (ActiveControl is PictureBox)
            {
                Clipboard.SetImage(((PictureBox)ActiveControl).Image);
            }
        }

        public override void Paste()
        {
            if (ActiveControl is TextBox)
            {
                ((TextBox)ActiveControl).Paste();
            }
        }

        public void UpdateFlags()
        {
            this.updating = true;
            IZ80 cpu = debugger.CPU;
            string bin = Convert.ToString(cpu.AF, 2);
            while (bin.Length < 16)
            {
                bin = '0' + bin;
            }

            bin = bin.Substring(8, 8);
            cflagBox.Checked = bin[7] != '0';
            nflagBox.Checked = bin[6] != '0';
            pvflagBox.Checked = bin[5] != '0';
            hcflagBox.Checked = bin[3] != '0';
            zflagBox.Checked = bin[1] != '0';
            sflagBox.Checked = bin[0] != '0';
            this.updating = false;
        }

        public void UpdateRegisters()
        {
            IZ80 cpu = debugger.CPU;
            afBox.Text = cpu.AF.ToString("X4");
            afpBox.Text = cpu.AFP.ToString("X4");
            bcBox.Text = cpu.BC.ToString("X4");
            bcpBox.Text = cpu.BCP.ToString("X4");
            deBox.Text = cpu.DE.ToString("X4");
            depBox.Text = cpu.DEP.ToString("X4");
            hlBox.Text = cpu.HL.ToString("X4");
            hlpBox.Text = cpu.HLP.ToString("X4");
            ixBox.Text = cpu.IX.ToString("X4");
            iyBox.Text = cpu.IY.ToString("X4");
            pcBox.Text = cpu.PC.ToString("X4");
            spBox.Text = cpu.SP.ToString("X4");
        }

        public void UpdateScreen()
        {
            screenPicBox.Image = debugger.GetScreenImage();
        }

        private void Copy(object sender, EventArgs e)
        {
            this.Copy();
        }

        private void gotoSourceMenuItem_Click(object sender, EventArgs e)
        {
            ContextMenu menu = ((MenuItem)sender).GetContextMenu();
            TextBox box = (TextBox)menu.SourceControl;
            ushort address = ushort.Parse(box.Text, System.Globalization.NumberStyles.HexNumber);
            byte page = debugger.GetPageNum(address);
            ListFileKey key = debugger.GetListKey(address, page);
            if (key == null)
            {
                return;
            }

            DocumentService.GotoLine(key.FileName, key.LineNumber);
        }

        private void Paste(object sender, EventArgs e)
        {
            this.Paste();
        }

        private void RegisterBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (debugger != null)
            {
                return;
            }

            if (e.KeyChar == (char)Keys.Enter)
            {
                IZ80 cpu = debugger.CPU;
                try
                {
                    cpu.AF = Convert.ToUInt16(afBox.Text, 16);
                    cpu.BC = Convert.ToUInt16(bcBox.Text, 16);
                    cpu.DE = Convert.ToUInt16(deBox.Text, 16);
                    cpu.HL = Convert.ToUInt16(hlBox.Text, 16);
                    cpu.IX = Convert.ToUInt16(ixBox.Text, 16);
                    cpu.IY = Convert.ToUInt16(iyBox.Text, 16);
                    cpu.AFP = Convert.ToUInt16(afpBox.Text, 16);
                    cpu.BCP = Convert.ToUInt16(bcpBox.Text, 16);
                    cpu.DEP = Convert.ToUInt16(depBox.Text, 16);
                    cpu.HLP = Convert.ToUInt16(hlpBox.Text, 16);
                    cpu.SP = Convert.ToUInt16(spBox.Text, 16);
                    cpu.PC = Convert.ToUInt16(pcBox.Text, 16);
                }
                catch { }
                this.UpdateRegisters();
            }

            if (e.KeyChar == (char)Keys.Cancel || e.KeyChar == (char)Keys.Escape)
            {
                this.UpdateRegisters();
            }
        }

        private void registersBox_TextChanged(object sender, EventArgs e)
        {
        }

        private void zflagBox_CheckedChanged(object sender, EventArgs e)
        {
            if (this.updating || debugger != null)
            {
                return;
            }

            IZ80 cpu = debugger.CPU;
            ushort f = (ushort)(Convert.ToUInt16(cflagBox.Checked) + Convert.ToUInt16(nflagBox.Checked) * 2 +
                                Convert.ToUInt16(pvflagBox.Checked) * 4 + 8 + Convert.ToUInt16(hcflagBox.Checked) * 16 +
                                32 + Convert.ToUInt16(zflagBox.Checked) * 64 + Convert.ToUInt16(sflagBox.Checked) * 128);
            cpu.AF &= 0xFF00;
            cpu.AF |= f;
            this.UpdateFlags();
        }
    }
}