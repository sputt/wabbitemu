using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Utils;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Properties;
using System;
using System.Configuration;
using System.Drawing;
using System.Drawing.Text;
using System.IO;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode
{
	/// <summary>
	/// Description of Preferences.
	/// </summary>
	public partial class Preferences : Form
	{
		private bool _browsing;
		private bool _changing;
		private readonly int[] _fontSizes = { 6, 8, 9, 10, 12, 14, 16, 18, 24 };
		private bool _savesettings;
		private readonly IDockingService _dockingService;

		public Preferences(IDockingService dockingService)
		{
			_dockingService = dockingService;

			foreach (SettingsProperty setting in Settings.Default.Properties)
			{
				TempSettings.Default[setting.Name] = Settings.Default[setting.Name];
			}

			InitializeComponent();
			InstalledFontCollection f = new InstalledFontCollection();
			foreach (FontFamily family in f.Families)
			{
				fontBox.Items.Add(family.Name);
				outFontBox.Items.Add(family.Name);
			}

			f.Dispose();
			foreach (int num in _fontSizes)
			{
				fontSizeBox.Items.Add(num);
				outFontSizeBox.Items.Add(num);
			}

			UpdateOptions();
		}

		private void UpdateOptions()
		{
			// editorbox font stuff
			_changing = true;
			fontBox.SelectedIndex = 0;
			string fontName = Settings.Default.editorFont.Name;
			while (fontBox.Items[fontBox.SelectedIndex].ToString() != fontName &&
				   fontBox.SelectedIndex < fontBox.Items.Count - 1)
			{
				fontBox.SelectedIndex++;
			}

			fontSizeBox.SelectedIndex = 0;
			while (fontSizeBox.Items[fontSizeBox.SelectedIndex].ToString() !=
				   Settings.Default.editorFont.Size.ToString() &&
				   fontSizeBox.SelectedIndex < fontSizeBox.Items.Count - 1)
			{
				fontSizeBox.SelectedIndex++;
			}

			// output window font stuff
			outFontBox.SelectedIndex = 0;
			string outFontName = Settings.Default.outputFont.Name;
			while (outFontBox.Items[outFontBox.SelectedIndex].ToString() != outFontName &&
				   outFontBox.SelectedIndex < outFontBox.Items.Count - 1)
			{
				outFontBox.SelectedIndex++;
			}

			outFontSizeBox.SelectedIndex = 0;
			while (outFontSizeBox.Items[outFontSizeBox.SelectedIndex].ToString() !=
				   Settings.Default.outputFont.Size.ToString() &&
				   outFontSizeBox.SelectedIndex < outFontSizeBox.Items.Count - 1)
			{
				outFontSizeBox.SelectedIndex++;
			}

			_changing = false;

			// checkboxs for editor
			autoIndentBox.Checked = Settings.Default.autoIndent;
			antiAliasBox.Checked = Settings.Default.antiAlias;
			inverseScrollingBox.Checked = Settings.Default.inverseScrolling;
			enableFoldingBox.Checked = Settings.Default.enableFolding;
			enableAutoTriggerBox.Checked = Settings.Default.enableAutoTrigger;
			enableRefHighlighterBox.Checked = Settings.Default.referencesHighlighter;
			colorizeOutWinBox.Checked = Settings.Default.colorizeOutWin;

			// highlighting stuff
			highlightEnabled.Checked = Settings.Default.useSyntaxHighlighting;
			highlightBox.Enabled = Settings.Default.useSyntaxHighlighting;
			defaultProjBox.Text = Settings.Default.startupProject;

			// this is fucking badass code right here
			foreach (Control control in highlightBox.Controls)
			{
				var box = control as CheckBox;
				if (box != null)
				{
					box.Checked = (bool)Settings.Default[box.Name];
				}

				if (control is Button)
				{
					control.BackColor = (Color)Settings.Default[control.Name];
				}
			}

			// output stuff
			foreach (Control control in outTypeBox.Controls)
			{
				var button = control as RadioButton;
				if (button != null)
				{
					button.Checked = Settings.Default.outputFile == button.TabIndex;
				}
			}

			caseSenseBox.Checked = Settings.Default.caseSensitive;

			// need to add this eventually
			noBinaryBox.Checked = false;
			sendFileEmu.Checked = Settings.Default.sendFileEmu;
			tabSizeUpDown.Value = Settings.Default.tabSize;
			saveAutoBox.Checked = Settings.Default.autoSaveProject;
			convertTabsToSpacesBox.Checked = Settings.Default.convertTabs;
			wabbitFolderCheck.Checked = Settings.Default.createFolders;
			assemblerLocBox.Text = Environment.ExpandEnvironmentVariables(
									   Settings.Default.assemblerLoc.Replace(
										   "%docs%",
										   Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)));
			dockConfigLoc.Text = Environment.ExpandEnvironmentVariables(
									 Settings.Default.wabbitLoc.Replace(
										 "%docs%",
										 Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)));
		}

		private void antiAliasBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.antiAlias = antiAliasBox.Checked;
		}

		private void applyButton_Click(object sender, EventArgs e)
		{
			_savesettings = true;
			UpdateSettings();
			_savesettings = false;
		}

		private void autoIndentBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.autoIndent = autoIndentBox.Checked;
		}

		private void browseAssembler_Click(object sender, EventArgs e)
		{
			var openFile = new OpenFileDialog
			{
				Filter = "Executables (*.exe)|*.exe",
				Title = "SPASM Location"
			};
			if (openFile.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			assemblerLocBox.Text = openFile.FileName;
			TempSettings.Default.assemblerLoc = openFile.FileName;
		}

		private void browseHighButton_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog
			{
				Filter = "Highlighting Files|*.xshd|All files|*.*",
				DefaultExt = ".xshd",
				FilterIndex = 0,
				RestoreDirectory = true,
				Title = "Open Highlighting File",
			};
			if (openFileDialog.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			_browsing = true;
			externalHighBox.Text = openFileDialog.FileName;
			TempSettings.Default.externalHighlight = openFileDialog.FileName;
		}

		private void browseProjButton_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog
			{
				Filter = "Project Files|*.wcodeproj|All files|*.*",
				DefaultExt = ".wcodeproj",
				FilterIndex = 0,
				RestoreDirectory = true,
				Title = "Open Project File",
			};
			if (openFileDialog.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			_browsing = true;
			defaultProjBox.Text = openFileDialog.FileName;
			TempSettings.Default.startupProject = openFileDialog.FileName;
		}

		private void browseWabbit_Click(object sender, EventArgs e)
		{
			var openFile = new OpenFileDialog
			{
				Filter = "Executables (*.exe)|*.exe",
				Title = "Wabbitemu Location"
			};
			if (openFile.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			wabbitLocBox.Text = openFile.FileName;
			TempSettings.Default.wabbitLoc = openFile.FileName;
		}

		private void cancelButton_Click(object sender, EventArgs e)
		{
			_savesettings = false;
		}

		private void caseSenseBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.caseSensitive = caseSenseBox.Checked;
		}

		private void convertTabsToSpacesBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.convertTabs = convertTabsToSpacesBox.Checked;
		}

		private void defaultProjBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			TempSettings.Default.startupProject = defaultProjBox.Text;
		}

		private void defaultProjBox_TextChanged(object sender, EventArgs e)
		{
			TempSettings.Default.startupProject = defaultProjBox.Text;
		}

		private void dockConfigBrowse_Click(object sender, EventArgs e)
		{
			var openFile = new OpenFileDialog
			{
				Filter = "Configuration files (*.config)|*.config",
				Title = "Dock Panel Config Location"
			};
			if (openFile.ShowDialog() != DialogResult.OK)
			{
				return;
			}

			dockConfigLoc.Text = openFile.FileName;
			TempSettings.Default.configLoc = openFile.FileName;
		}

		private void enableAutoCompleteBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.enableAutoTrigger = enableAutoTriggerBox.Checked;
		}

		private void enableFoldingBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.enableFolding = enableFoldingBox.Checked;
		}

		private void enableRefHighlighterBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.referencesHighlighter = enableRefHighlighterBox.Checked;
		}

		private void fontBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (fontSizeBox.SelectedIndex < 0 || fontBox.SelectedIndex < 0)
			{
				return;
			}

			TempSettings.Default.editorFont = new Font(
				new FontFamily(fontBox.Items[fontBox.SelectedIndex].ToString()),
				Convert.ToInt16(
					fontSizeBox.Items[fontSizeBox.SelectedIndex].ToString()));
		}

		private void fontSizeBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (fontSizeBox.SelectedIndex < 0 || fontBox.SelectedIndex < 0 || _changing)
			{
				return;
			}

			TempSettings.Default.editorFont = new Font(
				new FontFamily(fontBox.Items[fontBox.SelectedIndex].ToString()),
				Convert.ToInt16(
					fontSizeBox.Items[fontSizeBox.SelectedIndex].ToString()));
		}

		private void HighlightCheckChange(object sender, EventArgs e)
		{
			CheckBox control = (CheckBox)sender;
			TempSettings.Default[control.Name] = control.Checked;
		}

		private void highlightEnabled_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.useSyntaxHighlighting = highlightEnabled.Checked;
			highlightBox.Enabled = highlightEnabled.Checked;
		}

		private void labelColor_Click(object sender, EventArgs e)
		{
			Button colorLabel = (Button)sender;
			colorDialog.Color = colorLabel.BackColor;
			if (colorDialog.ShowDialog() == DialogResult.OK)
			{
				colorLabel.BackColor = colorDialog.Color;
				TempSettings.Default[colorLabel.Name] = colorDialog.Color;
			}
		}

		private void okButton_Click(object sender, EventArgs e)
		{
			_savesettings = true;
		}

		private void outFontBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (outFontSizeBox.SelectedIndex < 0 || outFontBox.SelectedIndex < 0)
			{
				return;
			}

			TempSettings.Default.outputFont = new Font(
				new FontFamily(fontBox.Items[fontBox.SelectedIndex].ToString()),
				Convert.ToInt16(
					fontSizeBox.Items[fontSizeBox.SelectedIndex].ToString()));
		}

		private void outFontSizeBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (outFontSizeBox.SelectedIndex < 0 || outFontBox.SelectedIndex < 0 || _changing)
			{
				return;
			}

			TempSettings.Default.outputFont = new Font(
				new FontFamily(fontBox.Items[fontBox.SelectedIndex].ToString()),
				Convert.ToInt16(
					fontSizeBox.Items[fontSizeBox.SelectedIndex].ToString()));
		}

		private void Preferences_FormClosing(object sender, FormClosingEventArgs e)
		{
			if (_browsing)
			{
				_browsing = false;
				e.Cancel = true;
			}

			UpdateSettings();
		}

		private void saveAutoBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.autoSaveProject = saveAutoBox.Checked;
		}

		private void sendFileEmu_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.sendFileEmu = sendFileEmu.Checked;
		}

		private void tabSizeUpDown_ValueChanged(object sender, EventArgs e)
		{
			TempSettings.Default.tabSize = (int)tabSizeUpDown.Value;
		}

		private void ti83Radio_CheckedChanged(object sender, EventArgs e)
		{
			RadioButton outputRadio = (RadioButton)sender;
			TempSettings.Default.outputFile = outputRadio.TabIndex;
		}

		private void UpdateSettings()
		{
			if (_savesettings)
			{
				foreach (SettingsProperty setting in TempSettings.Default.Properties)
				{
					Settings.Default[setting.Name] = TempSettings.Default[setting.Name];
				}

				if (!string.IsNullOrEmpty(Settings.Default.externalHighlight))
				{
					FileSyntaxModeProvider fsmProvider = new FileSyntaxModeProvider(Path.GetDirectoryName(Settings.Default.externalHighlight));
					HighlightingManager.Manager.AddSyntaxModeFileProvider(fsmProvider);
				}

				_dockingService.OutputWindow.UpdateFont(Settings.Default.outputFont);
				HighlightingUtils.MakeHighlightingFile();
				foreach (NewEditor child in _dockingService.Documents)
				{
					child.UpdateOptions(TempSettings.Default);

					// if (child.FileName != null)
					//	child.editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(child.FileName);
					child.SetHighlighting("Z80 Assembly");

				}
			}
		}

		private void wabbitFolderCheck_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.createFolders = wabbitFolderCheck.Checked;
		}

		// private void checkUpdatesBox_CheckedChanged(object sender, EventArgs e)
		// {
		//    TempSettings.Default.checkUpdates = checkUpdatesBox.Checked;
		// }
	}
}