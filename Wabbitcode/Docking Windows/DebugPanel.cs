using System;
using System.Windows.Forms;
using Revsoft.Docking;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using System.Drawing;
using System.Runtime.InteropServices;

namespace Revsoft.Wabbitcode.Docking_Windows
{
	public partial class DebugPanel : ToolWindow
	{
		public DebugPanel()
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
		}

		private void registersBox_TextChanged(object sender, EventArgs e)
		{
			
		}

		private void gotoSourceMenuItem_Click(object sender, EventArgs e)
		{
			ContextMenu menu = ((MenuItem) sender).GetContextMenu();
			TextBox box = (TextBox) menu.SourceControl;
			ushort address = ushort.Parse(box.Text, System.Globalization.NumberStyles.HexNumber);
			byte page = DebuggerService.GetPageNum(address);
			ListFileKey key = DebuggerService.GetListKey(address, page);
			if (key == null)
				return;
			DocumentService.GotoLine(key.FileName, key.LineNumber);
		}

		public void UpdateRegisters()
		{
#if NEW_DEBUGGING
			//HACK: I just wanna make this work :P
			CWabbitemu.Z80_State state = new CWabbitemu.Z80_State();// GetState(0);
#else
			CWabbitemu.Z80_State state = DebuggerService.Debugger.getState();
#endif
			afBox.Text = state.AF.ToString("X4");
			afpBox.Text = state.AFP.ToString("X4");
			bcBox.Text = state.BC.ToString("X4");
			bcpBox.Text = state.BCP.ToString("X4");
			deBox.Text = state.DE.ToString("X4");
			depBox.Text = state.DEP.ToString("X4");
			hlBox.Text = state.HL.ToString("X4");
			hlpBox.Text = state.HLP.ToString("X4");
			ixBox.Text = state.IX.ToString("X4");
			iyBox.Text = state.IY.ToString("X4");
			pcBox.Text = state.PC.ToString("X4");
			spBox.Text = state.SP.ToString("X4");
		}

		IntPtr scan0 = Marshal.AllocHGlobal(128 * 64);
		public void UpdateScreen()
		{
#if NEW_DEBUGGING
			var calcBitmap = new Bitmap(128, 64, 128, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, scan0);
			var palette = calcBitmap.Palette;
			for (int i = 0; i < 255; i++)
			{
				palette.Entries[i] = Color.FromArgb(0x9e * (256 - i) / 255, (0xAB * (256 - i)) / 255, (0x88 * (256 - i)) / 255);
			}
			calcBitmap.Palette = palette;
			screenPicBox.Image = calcBitmap;
#else
			screenPicBox.Image = DebuggerService.Debugger.DrawScreen();
#endif
		}

		bool updating;
		public void UpdateFlags()
		{
			updating = true;
#if NEW_DEBUGGING
			//HACK: I just wanna make this work :P
			CWabbitemu.Z80_State state = new CWabbitemu.Z80_State();// GetState(0);
#else
			CWabbitemu.Z80_State state = DebuggerService.Debugger.getState();
#endif
			string bin = Convert.ToString(state.AF, 2);
			while (bin.Length < 16)
				bin = '0' + bin;
			bin = bin.Substring(8, 8);
			cflagBox.Checked = bin[7] != '0';
			nflagBox.Checked = bin[6] != '0';
			pvflagBox.Checked = bin[5] != '0';
			hcflagBox.Checked = bin[3] != '0';
			zflagBox.Checked = bin[1] != '0';
			sflagBox.Checked = bin[0] != '0';
			updating = false;
		}

		private void RegisterBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (!DebuggerService.IsDebugging)
				return;
			if (e.KeyChar == (char)Keys.Enter)
			{
#if NEW_DEBUGGING
				//HACK: I just wanna make this work :P
				CWabbitemu.Z80_State state = new CWabbitemu.Z80_State();// GetState(0);
#else
				CWabbitemu.Z80_State state = DebuggerService.Debugger.getState();
#endif
				try
				{
					state.AF = Convert.ToUInt16(afBox.Text, 16);
					state.BC = Convert.ToUInt16(bcBox.Text, 16);
					state.DE = Convert.ToUInt16(deBox.Text, 16);
					state.HL = Convert.ToUInt16(hlBox.Text, 16);
					state.IX = Convert.ToUInt16(ixBox.Text, 16);
					state.IY = Convert.ToUInt16(iyBox.Text, 16);
					state.AFP = Convert.ToUInt16(afpBox.Text, 16);
					state.BCP = Convert.ToUInt16(bcpBox.Text, 16);
					state.DEP = Convert.ToUInt16(depBox.Text, 16);
					state.HLP = Convert.ToUInt16(hlpBox.Text, 16);
					state.SP = Convert.ToUInt16(spBox.Text, 16);
					state.PC = Convert.ToUInt16(pcBox.Text, 16);
#if NEW_DEBUGGING
				//HACK: I just wanna make this work :P
				//SetState(0, state);
#else
					DebuggerService.Debugger.setState(state);
#endif
				}
				catch { }
				UpdateRegisters();
			}
			if (e.KeyChar == (char)Keys.Cancel || e.KeyChar == (char)Keys.Escape)
				UpdateRegisters();
		}

		private void zflagBox_CheckedChanged(object sender, EventArgs e)
		{
			if (updating || !DebuggerService.IsDebugging)
				return;
#if NEW_DEBUGGING
			//HACK: I just wanna make this work :P
			CWabbitemu.Z80_State state = new CWabbitemu.Z80_State();// GetState(0);
#else
			CWabbitemu.Z80_State state = DebuggerService.Debugger.getState();
#endif
			try
			{
				ushort f = (ushort)(Convert.ToUInt16(cflagBox.Checked) + Convert.ToUInt16(nflagBox.Checked) * 2 +
					Convert.ToUInt16(pvflagBox.Checked) * 4 + 8 + Convert.ToUInt16(hcflagBox.Checked) * 16 +
					32 + Convert.ToUInt16(zflagBox.Checked) * 64 + Convert.ToUInt16(sflagBox.Checked) * 128);
				state.AF &= 0xFF00;
				state.AF |= f;
#if NEW_DEBUGGING
				//HACK: I just wanna make this work :P
				//SetState(0, state);
#else
				DebuggerService.Debugger.setState(state);
#endif
			}
			catch { }
			UpdateFlags();
		}

		private void Copy(object sender, EventArgs e)
		{
			Copy();
		}

		private void Paste(object sender, EventArgs e)
		{
			Paste();
		}

		public override void Copy()
		{
			if (ActiveControl.GetType() == typeof(TextBox))
				((TextBox)ActiveControl).Copy();
			else if (ActiveControl.GetType() == typeof(PictureBox))
				Clipboard.SetImage(((PictureBox)ActiveControl).Image);
		}

		public override void Cut()
		{
			if (ActiveControl.GetType() == typeof(TextBox))
				((TextBox)ActiveControl).Cut();
			else if (ActiveControl.GetType() == typeof(PictureBox))
				Clipboard.SetImage(((PictureBox)ActiveControl).Image);
		}

		public override void Paste()
		{
			if (ActiveControl.GetType() == typeof(TextBox))
				((TextBox)ActiveControl).Paste();
		}
		//private void updateCPUStatus()
		//{
		//    Wabbitemu.Z80_State state = debugger.getState();
		//    busBox.Text = state.Bus.ToString();
		//    freqBox.Text = state.Freq.ToString();
		//    haltBox.Checked = Convert.ToBoolean(state.Halt);

		//}
		//private void updateInterrupts()
		//{
		//    Wabbitemu.Z80_State state = debugger.getState();
		//    iff1Box.Checked = Convert.ToBoolean(state.IFF1);
		//    iff2Box.Checked = Convert.ToBoolean(state.IFF2);
		//    imBox.SelectedIndex = state.IMode;
		//}
	}
}