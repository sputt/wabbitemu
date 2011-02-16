using System;
using System.Collections.Generic;
using System.Configuration;
using System.Drawing;
using System.Drawing.Text;
using System.IO;
using System.Windows.Forms;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode
{
    /// <summary>
    /// Description of Preferences.
    /// </summary>
    public partial class Preferences : Form
    {
        private bool changing = false;
        private bool savesettings = false;
		int[] fontSizes = { 6, 8, 9, 10, 12, 14, 16, 18, 24 };

        public Preferences()
        {
            //
            // The InitializeComponent() call is required for Windows Forms designer support.
            //
            foreach (SettingsProperty setting in Settings.Default.Properties)
                TempSettings.Default[setting.Name] = Settings.Default[setting.Name];
            InitializeComponent();
            InstalledFontCollection f = new InstalledFontCollection();
            foreach (FontFamily family in f.Families)
            {
                fontBox.Items.Add(family.Name);
                outFontBox.Items.Add(family.Name);
            }
            foreach (int num in fontSizes)
            {
                fontSizeBox.Items.Add(num);
                outFontSizeBox.Items.Add(num);
            }
            UpdateOptions();
        }

        public void UpdateOptions()
        {
            //editorbox font stuff
            changing = true;
            fontBox.SelectedIndex = 0;
            string fontName = Settings.Default.editorFont.Name;
            while (fontBox.Items[fontBox.SelectedIndex].ToString() != fontName &&
                   fontBox.SelectedIndex < fontBox.Items.Count - 1)
                fontBox.SelectedIndex++;
            fontSizeBox.SelectedIndex = 0;
            while (fontSizeBox.Items[fontSizeBox.SelectedIndex].ToString() !=
                   Settings.Default.editorFont.Size.ToString() &&
                   fontSizeBox.SelectedIndex < fontSizeBox.Items.Count - 1)
                fontSizeBox.SelectedIndex++;
            //output window font stuff
            outFontBox.SelectedIndex = 0;
            string outFontName = Settings.Default.outputFont.Name;
            while (outFontBox.Items[outFontBox.SelectedIndex].ToString() != outFontName &&
                   outFontBox.SelectedIndex < outFontBox.Items.Count - 1)
                outFontBox.SelectedIndex++;
            outFontSizeBox.SelectedIndex = 0;
            while (outFontSizeBox.Items[outFontSizeBox.SelectedIndex].ToString() !=
                   Settings.Default.outputFont.Size.ToString() &&
                   outFontSizeBox.SelectedIndex < outFontSizeBox.Items.Count - 1)
                outFontSizeBox.SelectedIndex++;
            changing = false;
            //checkboxs for editor
            autoIndentBox.Checked = Settings.Default.autoIndent;
            antiAliasBox.Checked = Settings.Default.antiAlias;
            inverseScrollingBox.Checked = Settings.Default.inverseScrolling;
            enableFoldingBox.Checked = Settings.Default.enableFolding;
            enableAutoTriggerBox.Checked = Settings.Default.enableAutoTrigger;
            enableRefHighlighterBox.Checked = Settings.Default.referencesHighlighter;
            colorizeOutWinBox.Checked = Settings.Default.colorizeOutWin;
            //highlighting stuff
            highlightEnabled.Checked = Settings.Default.useSyntaxHighlighting;
            highlightBox.Enabled = Settings.Default.useSyntaxHighlighting;
            defaultProjBox.Text = Settings.Default.startupProject;
            //this is fucking badass code right here
            foreach (Control control in highlightBox.Controls)
            {
                if (control.GetType() == typeof (CheckBox))
                    ((CheckBox) control).Checked = (bool) Settings.Default[control.Name];
                if (control.GetType() == typeof (Button))
                    control.BackColor = (Color) Settings.Default[control.Name];
            }
            //output stuff
            foreach (Control control in outTypeBox.Controls)
            {
                if (control.GetType() == typeof (RadioButton))
                    ((RadioButton)control).Checked = Settings.Default.outputFile == control.TabIndex;
            }
            caseSenseBox.Checked = Settings.Default.caseSensitive;
            //need to add this eventually
            noBinaryBox.Checked = false;
            sendFileEmu.Checked = Settings.Default.sendFileEmu;
            tabSizeUpDown.Value = Settings.Default.tabSize;
            saveAutoBox.Checked = Settings.Default.autoSaveProject;
            convertTabsToSpacesBox.Checked = Settings.Default.convertTabs;
        }

        private void fontBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (fontSizeBox.SelectedIndex < 0 || fontBox.SelectedIndex < 0)
                return;
            TempSettings.Default.editorFont = new Font(new FontFamily(fontBox.Items[fontBox.SelectedIndex].ToString()),
                                                   Convert.ToInt16(
                                                       fontSizeBox.Items[fontSizeBox.SelectedIndex].ToString()));
        }

        private void fontSizeBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (fontSizeBox.SelectedIndex < 0 || fontBox.SelectedIndex < 0 || changing)
                return;
            TempSettings.Default.editorFont = new Font(new FontFamily(fontBox.Items[fontBox.SelectedIndex].ToString()),
                                                   Convert.ToInt16(
                                                       fontSizeBox.Items[fontSizeBox.SelectedIndex].ToString()));
        }

        private void outFontBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (outFontSizeBox.SelectedIndex < 0 || outFontBox.SelectedIndex < 0)
                return;
            TempSettings.Default.outputFont = new Font(new FontFamily(fontBox.Items[fontBox.SelectedIndex].ToString()),
                                                   Convert.ToInt16(
                                                       fontSizeBox.Items[fontSizeBox.SelectedIndex].ToString()));
        }

        private void outFontSizeBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (outFontSizeBox.SelectedIndex < 0 || outFontBox.SelectedIndex < 0 || changing)
                return;
            TempSettings.Default.outputFont = new Font(new FontFamily(fontBox.Items[fontBox.SelectedIndex].ToString()),
                                                   Convert.ToInt16(
                                                       fontSizeBox.Items[fontSizeBox.SelectedIndex].ToString()));
        }

        private void labelColor_Click(object sender, EventArgs e)
        {
            Button colorLabel = (Button) sender;
            colorDialog.Color = colorLabel.BackColor;
            if (colorDialog.ShowDialog() == DialogResult.OK)
            {
                colorLabel.BackColor = colorDialog.Color;
                TempSettings.Default[colorLabel.Name] = colorDialog.Color;
            }
        }

        private void highlightEnabled_CheckedChanged(object sender, EventArgs e)
        {
            TempSettings.Default.useSyntaxHighlighting = highlightEnabled.Checked;
            highlightBox.Enabled = highlightEnabled.Checked;
        }

        private void autoIndentBox_CheckedChanged(object sender, EventArgs e)
        {
            TempSettings.Default.autoIndent = autoIndentBox.Checked;
        }

        private void antiAliasBox_CheckedChanged(object sender, EventArgs e)
        {
            TempSettings.Default.antiAlias = antiAliasBox.Checked;
        }

        private void mouseWheelZoomBox_CheckedChanged(object sender, EventArgs e)
        {
            TempSettings.Default.inverseScrolling = inverseScrollingBox.Checked;
        }

        private void enableFoldingBox_CheckedChanged(object sender, EventArgs e)
        {
            TempSettings.Default.enableFolding = enableFoldingBox.Checked;
        }

        private void enableAutoCompleteBox_CheckedChanged(object sender, EventArgs e)
        {
            TempSettings.Default.enableAutoTrigger = enableAutoTriggerBox.Checked;
            /*TempSettings.Default.enableAutoComplete = enableAutoCompleteBox.Checked;
            foreach (newEditor child in parentForm.MdiChildren)
            {
            }
            if (!enableAutoCompleteBox.Checked)
            {
                enableAutoCompleteBox.Checked = true;
                MessageBox.Show("Hmm...i dont know enough about the text editor to remove this. its too cool to be disabled anyway");
            }*/
        }

        private void enableRefHighlighterBox_CheckedChanged(object sender, EventArgs e)
        {
            TempSettings.Default.referencesHighlighter = enableRefHighlighterBox.Checked;
        }

        private void labelsEnabled_CheckedChanged(object sender, EventArgs e)
        {
            var box = (CheckBox) sender;
            TempSettings.Default[box.Name] = box.Checked;
        }

        private void ti83Radio_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton outputRadio = (RadioButton) sender;
            TempSettings.Default.outputFile = outputRadio.TabIndex;
        }

        private void caseSenseBox_CheckedChanged(object sender, EventArgs e)
        {
            TempSettings.Default.caseSensitive = caseSenseBox.Checked;
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            savesettings = true;
        }

		private void UpdateSettings()
		{
			if (savesettings)
			{
				foreach (SettingsProperty setting in TempSettings.Default.Properties)
					Settings.Default[setting.Name] = TempSettings.Default[setting.Name];
				if (!string.IsNullOrEmpty(Settings.Default.externalHighlight))
				{
					FileSyntaxModeProvider fsmProvider = new FileSyntaxModeProvider(Path.GetDirectoryName(Settings.Default.externalHighlight));
					HighlightingManager.Manager.AddSyntaxModeFileProvider(fsmProvider);
				}
				DockingService.OutputWindow.UpdateFont(Settings.Default.outputFont);
				HighlightingClass.MakeHighlightingFile();
				foreach (newEditor child in DockingService.Documents)
				{
                    child.UpdateOptions(TempSettings.Default);
					//if (child.FileName != null)
					//	child.editorBox.Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(child.FileName);
					child.SetHighlighting("Z80 Assembly");

				}
			}

		}

        private void Preferences_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (browsing)
            {
                browsing = false;
                e.Cancel = true;
			}
			UpdateSettings();
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            savesettings = false;
        }

        private void sendFileEmu_CheckedChanged(object sender, EventArgs e)
        {
            TempSettings.Default.sendFileEmu = sendFileEmu.Checked;
        }

        bool browsing;
        private void browseProjButton_Click(object sender, EventArgs e)
        {
			OpenFileDialog openFileDialog = new OpenFileDialog()
			{
				Filter = "Project Files|*.wcodeproj|All files|*.*",
				DefaultExt = ".wcodeproj",
				FilterIndex = 0,
				RestoreDirectory = true,
				Title = "Open Project File",
			};
            if (openFileDialog.ShowDialog() != DialogResult.OK) return;
            browsing = true;
            defaultProjBox.Text = openFileDialog.FileName;
            TempSettings.Default.startupProject = openFileDialog.FileName;
        }

        private void defaultProjBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            TempSettings.Default.startupProject = defaultProjBox.Text;
        }

        private void browseHighButton_Click(object sender, EventArgs e)
        {
			OpenFileDialog openFileDialog = new OpenFileDialog()
			{
				Filter = "Highlighting Files|*.xshd|All files|*.*",
				DefaultExt = ".xshd",
				FilterIndex = 0,
				RestoreDirectory = true,
				Title = "Open Highlighting File",
			};
            if (openFileDialog.ShowDialog() != DialogResult.OK) return;
            browsing = true;
            externalHighBox.Text = openFileDialog.FileName;
            TempSettings.Default.externalHighlight = openFileDialog.FileName;
        }

        private void defaultProjBox_TextChanged(object sender, EventArgs e)
        {
            TempSettings.Default.startupProject = defaultProjBox.Text;
		}

		private void highlightCheckChange(object sender, EventArgs e)
		{
			CheckBox control = (CheckBox) sender;
			TempSettings.Default[control.Name] = control.Checked;
		}

		private void applyButton_Click(object sender, EventArgs e)
		{
			savesettings = true;
			UpdateSettings();
			savesettings = false;
		}

        private void saveAutoBox_CheckedChanged(object sender, EventArgs e)
        {
            TempSettings.Default.autoSaveProject = saveAutoBox.Checked;
        }

        private void tabSizeUpDown_ValueChanged(object sender, EventArgs e)
        {
            TempSettings.Default.tabSize = (int) tabSizeUpDown.Value;
        }

        private void convertTabsToSpacesBox_CheckedChanged(object sender, EventArgs e)
        {
            TempSettings.Default.convertTabs = convertTabsToSpacesBox.Checked;
        }
    }
}