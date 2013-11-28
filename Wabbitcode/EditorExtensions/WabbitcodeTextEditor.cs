using System.Collections.Generic;
using System.Configuration;
using System.Drawing;
using System.Drawing.Text;
using System.Windows.Forms;
using Revsoft.TextEditor;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Properties;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    internal sealed class WabbitcodeTextEditor : TextEditorControl
    {
        public WabbitcodeTextEditor()
        {
            AllowDrop = true;
            Dock = DockStyle.Fill;
            Font = Settings.Default.EditorFont;
            IsIconBarVisible = Settings.Default.IconBar;
            IsReadOnly = false;
            LineViewerStyle = Settings.Default.LineEnabled ?
                LineViewerStyle.FullRow : LineViewerStyle.None;
            ShowLineNumbers = Settings.Default.LineNumbers;
            ShowVRuler = false;
            TextEditorProperties.MouseWheelScrollDown = !Settings.Default.InverseScrolling;
            TextRenderingHint = Settings.Default.AntiAlias ?
                TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;

            Settings.Default.SettingChanging += Default_SettingChanging;
        }

        private void Default_SettingChanging(object sender, SettingChangingEventArgs e)
        {
            if (e.NewValue == null)
            {
                return;
            }

            switch (e.SettingName)
            {
                case "MouseWheelScrollDown":
                    TextEditorProperties.MouseWheelScrollDown = !((bool)e.NewValue);
                    break;
                case "EnableFolding":
                    if ((bool)e.NewValue)
                    {
                        Document.FoldingManager.FoldingStrategy = new RegionFoldingStrategy();
                        Document.FoldingManager.UpdateFoldings(null, null);
                    }
                    else
                    {
                        Document.FoldingManager.FoldingStrategy = null;
                        Document.FoldingManager.UpdateFoldings(new List<FoldMarker>());
                    }
                    break;
                case "AutoIndent":
                    IndentStyle = (bool)e.NewValue ? IndentStyle.Smart : IndentStyle.None;
                    break;
                case "AntiAlias":
                    TextRenderingHint = (bool)e.NewValue ? TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel;
                    break;
                case "EditorFont":
                    Font = (Font)e.NewValue;
                    break;
                case "TabSize":
                    TabIndent = (int)e.NewValue;
                    break;
                case "ConvertTabs":
                    ConvertTabsToSpaces = (bool)e.NewValue;
                    break;
                case "ExternalHighlight":
                    UpdateHighlighting();
                    break;
                case "IconBar":
                    IsIconBarVisible = (bool)e.NewValue;
                    break;
                case "LineNumbers":
                    ShowLineNumbers = (bool)e.NewValue;
                    break;
            }
        }

        private void UpdateHighlighting()
        {
            if (!string.IsNullOrEmpty(FileName))
            {
                Document.HighlightingStrategy = HighlightingStrategyFactory.CreateHighlightingStrategyForFile(FileName);
            }
            else
            {
                SetHighlighting("Z80 Assembly");
            }
        }
    }
}
