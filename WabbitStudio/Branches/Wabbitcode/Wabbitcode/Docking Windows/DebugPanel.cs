using System.Drawing;
using System.Drawing.Drawing2D;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Services.Interface;
using System;
using System.Windows.Forms;
using WabbitemuLib;


namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class DebugPanel : ToolWindow
	{
		private bool _updating;
		private IWabbitcodeDebugger _debugger;

		public DebugPanel(IDockingService dockingService)
			: base(dockingService)
		{
			InitializeComponent();

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
			dockingService.MainForm.OnDebuggingStarted += mainForm_OnDebuggingStarted;
		}

		void mainForm_OnDebuggingStarted(object sender, DebuggingEventArgs e)
		{
			_debugger = e.Debugger;
		}

		public override void Copy()
		{
			var textBox = ActiveControl as TextBox;
			if (textBox != null)
			{
				textBox.Copy();
			}
			else
			{
				var pictureBox = ActiveControl as PictureBox;
				if (pictureBox != null)
				{
					Clipboard.SetImage(pictureBox.Image);
				}
			}
		}

		public override void Cut()
		{
			var textBox = ActiveControl as TextBox;
			if (textBox != null)
			{
				textBox.Cut();
			}
			else
			{
				var pictureBox = ActiveControl as PictureBox;
				if (pictureBox != null)
				{
					Clipboard.SetImage(pictureBox.Image);
				}
			}
		}

		public override void Paste()
		{
			var textBox = ActiveControl as TextBox;
			if (textBox != null)
			{
				textBox.Paste();
			}
		}

		public void UpdateFlags()
		{
			_updating = true;
			IZ80 cpu = _debugger.CPU;
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
			_updating = false;
		}

		public void UpdateRegisters()
		{
			IZ80 cpu = _debugger.CPU;
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
			Image image = _debugger.ScreenImage;
			Image scaledImage = new Bitmap(image.Width * 2, image.Height * 2);
			Graphics graphics = Graphics.FromImage(scaledImage);
			graphics.InterpolationMode = InterpolationMode.NearestNeighbor;
			screenPicBox.Image = scaledImage;
			graphics.DrawImage(image, new Rectangle(0, 0, scaledImage.Width , scaledImage.Height),
				new Rectangle(0, 0, image.Width, image.Height), GraphicsUnit.Pixel);
		}

		private void Copy(object sender, EventArgs e)
		{
			Copy();
		}

		private void gotoSourceMenuItem_Click(object sender, EventArgs e)
		{
			ContextMenu menu = ((MenuItem)sender).GetContextMenu();
			if (menu == null)
			{
				return;
			}
			TextBox box = (TextBox)menu.SourceControl;
			ushort address = ushort.Parse(box.Text, System.Globalization.NumberStyles.HexNumber);
			_debugger.GotoAddress(address);
		}

		private void Paste(object sender, EventArgs e)
		{
			Paste();
		}

		private void RegisterBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (_debugger == null)
			{
				return;
			}

			if (e.KeyChar == (char)Keys.Enter)
			{
				IZ80 cpu = _debugger.CPU;
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
				UpdateRegisters();
			}

			if (e.KeyChar == (char)Keys.Cancel || e.KeyChar == (char)Keys.Escape)
			{
				UpdateRegisters();
			}
		}

		private void registersBox_TextChanged(object sender, EventArgs e)
		{
		}

		private void zflagBox_CheckedChanged(object sender, EventArgs e)
		{
			if (_updating || _debugger == null)
			{
				return;
			}

			IZ80 cpu = _debugger.CPU;
			ushort f = (ushort)(Convert.ToUInt16(cflagBox.Checked) + Convert.ToUInt16(nflagBox.Checked) * 2 +
								Convert.ToUInt16(pvflagBox.Checked) * 4 + 8 + Convert.ToUInt16(hcflagBox.Checked) * 16 +
								32 + Convert.ToUInt16(zflagBox.Checked) * 64 + Convert.ToUInt16(sflagBox.Checked) * 128);
			cpu.AF &= 0xFF00;
			cpu.AF |= f;
			UpdateFlags();
		}
	}
}