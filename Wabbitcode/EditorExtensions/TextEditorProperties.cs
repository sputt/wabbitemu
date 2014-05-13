using System;
using System.Drawing;
using System.Drawing.Text;
using System.Text;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Properties;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    public sealed class TextEditorProperties : ITextEditorProperties
    {
        private readonly FontContainer _fontContainer = new FontContainer(Settings.Default.EditorFont);

        public TextEditorProperties()
        {
            ShowVerticalRuler = false;
            Encoding = Encoding.UTF8;
            BracketMatchingStyle = BracketMatchingStyle.After;
            IndentationSize = 4;
            DocumentSelectionMode = DocumentSelectionMode.Normal;
            AllowCaretBeyondEOL = false;
            CaretLine = false;
            HideMouseCursor = false;
            ShowMatchingBracket = true;
            ShowSpaces = false;
            ShowEOLMarker = false;
            ShowInvalidLines = false;
            MouseWheelTextZoom = true;
            CutCopyWholeLine = true;
            VerticalRulerRow = 80;
            LineTerminator = Environment.NewLine;
            SupportReadOnlySegments = false;
            IconBarBackColor = SystemColors.Control;
            IconBarLineColor = SystemColors.ControlDark;
            IsSplit = false;
        }

        public bool CaretLine { get; set; }
        public bool AutoInsertCurlyBracket { get; set; }
        public bool HideMouseCursor { get; set; }

        public bool IsIconBarVisible
        {
            get { return Settings.Default.IconBar; }
            set { Settings.Default.IconBar = value; }
        }

        public bool AllowCaretBeyondEOL { get; set; }
        public bool ShowMatchingBracket { get; set; }
        public bool CutCopyWholeLine { get; set; }

        public TextRenderingHint TextRenderingHint
        {
            get { return Settings.Default.AntiAlias ? TextRenderingHint.ClearTypeGridFit : TextRenderingHint.SingleBitPerPixel; }
            set { Settings.Default.AntiAlias = value == TextRenderingHint.ClearTypeGridFit; }
        }

        public bool MouseWheelScrollDown
        {
            get { return !Settings.Default.InverseScrolling; }
            set { Settings.Default.InverseScrolling = !value; }
        }

        public bool MouseWheelTextZoom { get; set; }
        public string LineTerminator { get; set; }

        public LineViewerStyle LineViewerStyle
        {
            get { return Settings.Default.LineEnabled ? LineViewerStyle.FullRow : LineViewerStyle.None; }
            set { Settings.Default.LineEnabled = value == LineViewerStyle.FullRow; }
        }

        public bool ShowInvalidLines { get; set; }
        public int VerticalRulerRow { get; set; }
        public bool ShowSpaces { get; set; }
        public bool ShowTabs { get; set; }
        public bool ShowEOLMarker { get; set; }

        public bool ConvertTabsToSpaces
        {
            get { return Settings.Default.ConvertTabs; }
            set { Settings.Default.ConvertTabs = value; }
        }

        public bool ShowHorizontalRuler { get; set; }
        public bool ShowVerticalRuler { get; set; }
        public Encoding Encoding { get; set; }

        public bool EnableFolding
        {
            get { return Settings.Default.EnableFolding; }
            set { Settings.Default.EnableFolding = value; }
        }

        public bool ShowLineNumbers
        {
            get { return Settings.Default.LineNumbers; }
            set { Settings.Default.LineNumbers = value; }
        }

        public int TabIndent
        {
            get { return Settings.Default.TabSize; }
            set { Settings.Default.TabSize = value; }
        }

        public int IndentationSize { get; set; }

        public IndentStyle IndentStyle
        {
            get { return Settings.Default.AutoIndent ? IndentStyle.Smart : IndentStyle.None; }
            set { Settings.Default.AutoIndent = value == IndentStyle.Smart; }
        }

        public DocumentSelectionMode DocumentSelectionMode { get; set; }

        public Font Font
        {
            get { return Settings.Default.EditorFont; }
            set { Settings.Default.EditorFont = value; }
        }

        public FontContainer FontContainer
        {
            get
            {
                if (!_fontContainer.DefaultFont.Equals(Font))
                {
                    _fontContainer.DefaultFont = Font;
                }

                return _fontContainer;
            }
        }

        public BracketMatchingStyle BracketMatchingStyle { get; set; }
        public bool SupportReadOnlySegments { get; set; }
        public Color IconBarBackColor { get; set; }
        public Color IconBarLineColor { get; set; }
        public bool IsSplit { get; set; }
    }
}