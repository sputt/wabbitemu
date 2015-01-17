using System;
using System.Collections.Generic;
using System.Configuration;
using System.Drawing;
using System.Drawing.Text;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    /// <summary>
    /// Description of Preferences.
    /// </summary>
    public partial class Preferences : Form
    {
        private bool _browsing;
        private readonly List<int> _fontSizes = new List<int> {6, 8, 9, 10, 12, 14, 16, 18, 24};
        private bool _savesettings;
        private readonly Settings _tempSettings = new Settings();

        public Preferences()
        {
            foreach (SettingsProperty setting in Settings.Default.Properties)
            {
                _tempSettings[setting.Name] = Settings.Default[setting.Name];
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
            string fontName = Settings.Default.EditorFont.Name;
            float fontSize = Settings.Default.EditorFont.Size;
            fontBox.SelectedIndex = fontBox.Items.Cast<string>().ToList().FindIndex(f => f == fontName);
            fontSizeBox.SelectedIndex = _fontSizes.FindIndex(s => Math.Abs(fontSize - s) < float.Epsilon);

            // output window font stuff
            string outputFontName = Settings.Default.OutputFont.Name;
            float outputFontSize = Settings.Default.OutputFont.Size;
            outFontBox.SelectedIndex = fontBox.Items.Cast<string>().ToList().FindIndex(f => f == outputFontName);
            outFontSizeBox.SelectedIndex = _fontSizes.FindIndex(s => Math.Abs(outputFontSize - s) < float.Epsilon);

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
            foreach (var box in highlightBox.Controls.OfType<CheckBox>())
            {
                box.Checked = (bool) Settings.Default[box.Name];
            }

            foreach (var control in highlightBox.Controls.OfType<Button>())
            {
                control.BackColor = (Color) Settings.Default[control.Name];
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
            _tempSettings.AntiAlias = antiAliasBox.Checked;
        }

        private void applyButton_Click(object sender, EventArgs e)
        {
            _savesettings = true;
            UpdateSettings();
            _savesettings = false;
        }

        private void autoIndentBox_CheckedChanged(object sender, EventArgs e)
        {
            _tempSettings.AutoIndent = autoIndentBox.Checked;
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
            _tempSettings.AssemblerLoc = openFile.FileName;
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
        }

        private void browseProjButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                Filter = string.Format("Project Files|*{0}|All files|*.*", ProjectService.ProjectExtension),
                DefaultExt = ProjectService.ProjectExtension,
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
            _tempSettings.StartupProject = openFileDialog.FileName;
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
            _tempSettings.WabbitLoc = openFile.FileName;
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            _savesettings = false;
            Close();
        }

        private void caseSenseBox_CheckedChanged(object sender, EventArgs e)
        {
            _tempSettings.CaseSensitive = caseSenseBox.Checked;
        }

        private void convertTabsToSpacesBox_CheckedChanged(object sender, EventArgs e)
        {
            _tempSettings.ConvertTabs = convertTabsToSpacesBox.Checked;
        }

        private void defaultProjBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            _tempSettings.StartupProject = defaultProjBox.Text;
        }

        private void defaultProjBox_TextChanged(object sender, EventArgs e)
        {
            _tempSettings.StartupProject = defaultProjBox.Text;
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
            _tempSettings.ConfigLoc = openFile.FileName;
        }

        private void enableAutoCompleteBox_CheckedChanged(object sender, EventArgs e)
        {
            _tempSettings.EnableAutoTrigger = enableAutoTriggerBox.Checked;
        }

        private void enableFoldingBox_CheckedChanged(object sender, EventArgs e)
        {
            _tempSettings.EnableFolding = enableFoldingBox.Checked;
        }

        private void enableRefHighlighterBox_CheckedChanged(object sender, EventArgs e)
        {
            _tempSettings.ReferencesHighlighter = enableRefHighlighterBox.Checked;
        }

        private void fontBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (fontSizeBox.SelectedIndex < 0 || fontBox.SelectedIndex < 0)
            {
                return;
            }

            string fontName = fontBox.SelectedItem.ToString();
            _tempSettings.EditorFont = new Font(new FontFamily(fontName),
                Convert.ToInt32(fontSizeBox.SelectedItem.ToString()));
        }

        private void fontSizeBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (fontSizeBox.SelectedIndex < 0 || fontBox.SelectedIndex < 0)
            {
                return;
            }

            string fontName = fontBox.SelectedItem.ToString();
            _tempSettings.EditorFont = new Font(new FontFamily(fontName),
                Convert.ToInt32(fontSizeBox.SelectedItem.ToString()));
        }

        private void HighlightCheckChange(object sender, EventArgs e)
        {
            CheckBox control = (CheckBox) sender;
            _tempSettings[control.Name] = control.Checked;
        }

        private void highlightEnabled_CheckedChanged(object sender, EventArgs e)
        {
            _tempSettings.UseSyntaxHighlighting = highlightEnabled.Checked;
            highlightBox.Enabled = highlightEnabled.Checked;
        }

        private void labelColor_Click(object sender, EventArgs e)
        {
            Button colorLabel = (Button) sender;
            colorDialog.Color = colorLabel.BackColor;
            if (colorDialog.ShowDialog() != DialogResult.OK)
            {
                return;
            }

            colorLabel.BackColor = colorDialog.Color;
            _tempSettings[colorLabel.Name] = colorDialog.Color;
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            _savesettings = true;
            UpdateSettings();
            Close();
        }

        private void outFontBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (outFontSizeBox.SelectedIndex < 0 || outFontBox.SelectedIndex < 0)
            {
                return;
            }

            _tempSettings.OutputFont = new Font(
                new FontFamily(fontBox.SelectedItem.ToString()),
                Convert.ToInt32(fontSizeBox.SelectedItem.ToString()));
        }

        private void outFontSizeBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (outFontSizeBox.SelectedIndex < 0 || outFontBox.SelectedIndex < 0)
            {
                return;
            }

            _tempSettings.OutputFont = new Font(
                new FontFamily(fontBox.SelectedItem.ToString()),
                Convert.ToInt32(fontSizeBox.SelectedItem.ToString()));
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
            _tempSettings.AutoSaveProject = saveAutoBox.Checked;
        }

        private void sendFileEmu_CheckedChanged(object sender, EventArgs e)
        {
            _tempSettings.SendFileEmu = sendFileEmu.Checked;
        }

        private void tabSizeUpDown_ValueChanged(object sender, EventArgs e)
        {
            _tempSettings.TabSize = (int) tabSizeUpDown.Value;
        }

        private void ti83Radio_CheckedChanged(object sender, EventArgs e)
        {
            RadioButton outputRadio = (RadioButton) sender;
            _tempSettings.OutputFile = outputRadio.TabIndex;
        }

        private void UpdateSettings()
        {
            if (!_savesettings)
            {
                return;
            }

            if (!string.IsNullOrEmpty(Settings.Default.ExternalHighlight))
            {
                Settings.Default.ExternalHighlight = _tempSettings.ExternalHighlight;
                HighlightingManager.Manager.ClearSyntaxModeFileProvider();
                HighlightingUtils.MakeHighlightingFile();
                HighlightingManager.Manager.ReloadSyntaxModes();
            }

            foreach (SettingsProperty setting in _tempSettings.Properties)
            {
                Settings.Default[setting.Name] = _tempSettings[setting.Name];
            }

            HighlightingUtils.MakeHighlightingFile();
        }

        private void wabbitFolderCheck_CheckedChanged(object sender, EventArgs e)
        {
            _tempSettings.CreateFolders = wabbitFolderCheck.Checked;
        }

        private void inverseScrollingBox_CheckedChanged(object sender, EventArgs e)
        {
            _tempSettings.InverseScrolling = inverseScrollingBox.Checked;
        }

        private void externalHighBox_TextChanged(object sender, EventArgs e)
        {
            _tempSettings.ExternalHighlight = externalHighBox.Text;
        }

        // private void checkUpdatesBox_CheckedChanged(object sender, EventArgs e)
        // {
        //    tempSettings.checkUpdates = checkUpdatesBox.Checked;
        // }
    }
}