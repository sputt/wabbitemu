using System.Linq;
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

	    public Preferences()
		{
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
			string fontName = Settings.Default.EditorFont.Name;
			while (fontBox.Items[fontBox.SelectedIndex].ToString() != fontName &&
				   fontBox.SelectedIndex < fontBox.Items.Count - 1)
			{
				fontBox.SelectedIndex++;
			}

			fontSizeBox.SelectedIndex = 0;
			while (fontSizeBox.Items[fontSizeBox.SelectedIndex].ToString() !=
				   Settings.Default.EditorFont.Size.ToString() &&
				   fontSizeBox.SelectedIndex < fontSizeBox.Items.Count - 1)
			{
				fontSizeBox.SelectedIndex++;
			}

			// output window font stuff
			outFontBox.SelectedIndex = 0;
			string outFontName = Settings.Default.OutputFont.Name;
			while (outFontBox.Items[outFontBox.SelectedIndex].ToString() != outFontName &&
				   outFontBox.SelectedIndex < outFontBox.Items.Count - 1)
			{
				outFontBox.SelectedIndex++;
			}

			outFontSizeBox.SelectedIndex = 0;
			while (outFontSizeBox.Items[outFontSizeBox.SelectedIndex].ToString() !=
				   Settings.Default.OutputFont.Size.ToString() &&
				   outFontSizeBox.SelectedIndex < outFontSizeBox.Items.Count - 1)
			{
				outFontSizeBox.SelectedIndex++;
			}

			_changing = false;

			// checkboxs for editor
			autoIndentBox.Checked = Settings.Default.AutoIndent;
			antiAliasBox.Checked = Settings.Default.AntiAlias;
			inverseScrollingBox.Checked = Settings.Default.InverseScrolling;
			enableFoldingBox.Checked = Settings.Default.EnableFolding;
			enableAutoTriggerBox.Checked = Settings.Default.EnableAutoTrigger;
			enableRefHighlighterBox.Checked = Settings.Default.ReferencesHighlighter;
			colorizeOutWinBox.Checked = Settings.Default.ColorizeOutWin;

			// highlighting stuff
			highlightEnabled.Checked = Settings.Default.UseSyntaxHighlighting;
			highlightBox.Enabled = Settings.Default.UseSyntaxHighlighting;
			defaultProjBox.Text = Settings.Default.StartupProject;
		    externalHighBox.Text = Settings.Default.ExternalHighlight;

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
			foreach (var button in outTypeBox.Controls.OfType<RadioButton>())
			{
			    button.Checked = Settings.Default.OutputFile == button.TabIndex;
			}

			caseSenseBox.Checked = Settings.Default.CaseSensitive;

			// need to add this eventually
			noBinaryBox.Checked = false;
			sendFileEmu.Checked = Settings.Default.SendFileEmu;
			tabSizeUpDown.Value = Settings.Default.TabSize;
			saveAutoBox.Checked = Settings.Default.AutoSaveProject;
			convertTabsToSpacesBox.Checked = Settings.Default.ConvertTabs;
			wabbitFolderCheck.Checked = Settings.Default.CreateFolders;
			assemblerLocBox.Text = Environment.ExpandEnvironmentVariables(
									   Settings.Default.AssemblerLoc.Replace(
										   "%docs%",
										   Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)));
			dockConfigLoc.Text = Environment.ExpandEnvironmentVariables(
									 Settings.Default.WabbitLoc.Replace(
										 "%docs%",
										 Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments)));
		}

		private void antiAliasBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.AntiAlias = antiAliasBox.Checked;
		}

		private void applyButton_Click(object sender, EventArgs e)
		{
			_savesettings = true;
			UpdateSettings();
			_savesettings = false;
		}

		private void autoIndentBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.AutoIndent = autoIndentBox.Checked;
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
			TempSettings.Default.AssemblerLoc = openFile.FileName;
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
			TempSettings.Default.ExternalHighlight = openFileDialog.FileName;
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
			TempSettings.Default.StartupProject = openFileDialog.FileName;
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
			TempSettings.Default.WabbitLoc = openFile.FileName;
		}

		private void cancelButton_Click(object sender, EventArgs e)
		{
			_savesettings = false;
		}

		private void caseSenseBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.CaseSensitive = caseSenseBox.Checked;
		}

		private void convertTabsToSpacesBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.ConvertTabs = convertTabsToSpacesBox.Checked;
		}

		private void defaultProjBox_KeyPress(object sender, KeyPressEventArgs e)
		{
			TempSettings.Default.StartupProject = defaultProjBox.Text;
		}

		private void defaultProjBox_TextChanged(object sender, EventArgs e)
		{
			TempSettings.Default.StartupProject = defaultProjBox.Text;
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
			TempSettings.Default.ConfigLoc = openFile.FileName;
		}

		private void enableAutoCompleteBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.EnableAutoTrigger = enableAutoTriggerBox.Checked;
		}

		private void enableFoldingBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.EnableFolding = enableFoldingBox.Checked;
		}

		private void enableRefHighlighterBox_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.ReferencesHighlighter = enableRefHighlighterBox.Checked;
		}

		private void fontBox_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (fontSizeBox.SelectedIndex < 0 || fontBox.SelectedIndex < 0)
			{
				return;
			}

			TempSettings.Default.EditorFont = new Font(
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

			TempSettings.Default.EditorFont = new Font(
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
			TempSettings.Default.UseSyntaxHighlighting = highlightEnabled.Checked;
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

			TempSettings.Default.OutputFont = new Font(
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

			TempSettings.Default.OutputFont = new Font(
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
			TempSettings.Default.AutoSaveProject = saveAutoBox.Checked;
		}

		private void sendFileEmu_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.SendFileEmu = sendFileEmu.Checked;
		}

		private void tabSizeUpDown_ValueChanged(object sender, EventArgs e)
		{
			TempSettings.Default.TabSize = (int)tabSizeUpDown.Value;
		}

		private void ti83Radio_CheckedChanged(object sender, EventArgs e)
		{
			RadioButton outputRadio = (RadioButton)sender;
			TempSettings.Default.OutputFile = outputRadio.TabIndex;
		}

		private void UpdateSettings()
		{
		    if (!_savesettings)
		    {
		        return;
		    }

            if (!string.IsNullOrEmpty(TempSettings.Default.ExternalHighlight))
            {
                FileSyntaxModeProvider fsmProvider = new FileSyntaxModeProvider(Path.GetDirectoryName(TempSettings.Default.ExternalHighlight));
                HighlightingManager.Manager.AddSyntaxModeFileProvider(fsmProvider);
            }

		    foreach (SettingsProperty setting in TempSettings.Default.Properties)
		    {
		        Settings.Default[setting.Name] = TempSettings.Default[setting.Name];
		    }

            HighlightingUtils.MakeHighlightingFile();
		}

		private void wabbitFolderCheck_CheckedChanged(object sender, EventArgs e)
		{
			TempSettings.Default.CreateFolders = wabbitFolderCheck.Checked;
		}

		// private void checkUpdatesBox_CheckedChanged(object sender, EventArgs e)
		// {
		//    TempSettings.Default.checkUpdates = checkUpdatesBox.Checked;
		// }
	}
}