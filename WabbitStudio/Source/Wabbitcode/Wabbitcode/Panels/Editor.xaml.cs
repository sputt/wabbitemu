using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Threading;
using AvalonDock;
using ICSharpCode.AvalonEdit;
using ICSharpCode.AvalonEdit.Folding;
using ICSharpCode.AvalonEdit.Highlighting;
using ICSharpCode.AvalonEdit.Indentation;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Panels
{
    /// <summary>
    /// Interaction logic for Editor.xaml
    /// </summary>
    public partial class Editor : DocumentContent, IWabbitcodePanel
    {
        private string filePath;
        private TextEditor editor;
        FoldingManager foldingManager;
        AbstractFoldingStrategy foldingStrategy;
        public Editor()
        {
            InitializeComponent();
            editor = new TextEditor();
            editor.TextArea.Caret.PositionChanged += new EventHandler(Caret_PositionChanged);
            editor.Document.UndoStack.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(UndoStack_PropertyChanged);

            foldingManager = FoldingManager.Install(editor.TextArea);
            foldingStrategy = new AsmFoldingStrategy();

            this.Content = editor;
        }

        void UndoStack_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            switch (e.PropertyName)
            {
                case "IsOriginalFile":
                    this.Title = Path.GetFileName(filePath);
                    if (!editor.Document.UndoStack.IsOriginalFile)
                        this.Title += "*";
                    break;
            }
        }

        const string fileNameAttribute = "FileName";
        public override void RestoreLayout(System.Xml.XmlElement contentElement)
        {
            DocumentService.OpenDocument(this, contentElement.GetAttribute(fileNameAttribute));
            base.RestoreLayout(contentElement);
        }

        public override void SaveLayout(System.Xml.XmlWriter storeWriter)
        {
            storeWriter.WriteAttributeString(fileNameAttribute, filePath);
            base.SaveLayout(storeWriter);
        }

        public void OpenFile(string filename)
        {
            filePath = filename;
            editor.Load(filename);
            editor.SyntaxHighlighting = HighlightingManager.Instance.GetDefinitionByExtension(Path.GetExtension(filename));
        }

        public void SaveFile()
        {
            editor.Save(filePath);
            editor.Document.UndoStack.MarkAsOriginalFile();
            editor.SyntaxHighlighting = HighlightingManager.Instance.GetDefinitionByExtension(Path.GetExtension(filePath));
        }

        #region Editor Functions
        public void Undo()
        {
            editor.Undo();
        }

        public void Redo()
        {
            editor.Redo();
        }

        public void Cut()
        {
            editor.Cut();
        }

        public void Copy()
        {
            editor.Copy();
        }

        public void Paste()
        {
            editor.Paste();
        }

        public void SelectAll()
        {
            editor.SelectAll();
        }
        #endregion

        #region Events
        private void EditorDocument_Loaded(object sender, RoutedEventArgs e)
        {
            editor.WordWrap = PropertyService.GetEditorProperty("WordWrap");
            editor.ShowLineNumbers = PropertyService.GetEditorProperty("ShowLineNumbers");
            editor.FontFamily = PropertyService.GetEditorProperty("Font");
            editor.TextArea.IndentationStrategy = new DefaultIndentationStrategy();
            
            DispatcherTimer foldingUpdateTimer = new DispatcherTimer();
            foldingUpdateTimer.Interval = TimeSpan.FromSeconds(2);
            foldingUpdateTimer.Tick += foldingUpdateTimer_Tick;
            foldingUpdateTimer.Start();
            foldingUpdateTimer_Tick(null, EventArgs.Empty);
        }

        void Caret_PositionChanged(object sender, EventArgs e)
        {
            int line = editor.TextArea.Caret.Line;
            int chOffset = editor.TextArea.Caret.Column;
            int lineOffset = editor.Document.GetLineByNumber(line).Offset;
            int col = 1;
            for (int i = 1; i < chOffset; i++)
            {
                if (editor.Document.GetCharAt(lineOffset + i - 1) == '\t')
                    col += 4;
                else
                    col += 1;
            }
            DockingService.StatusBar.SetCaretPosition(col, line, chOffset);
        }

        private void EditorDocument_Closed(object sender, EventArgs e)
        {

        }

        void foldingUpdateTimer_Tick(object sender, EventArgs e)
        {
            if (foldingStrategy != null)
            {
                foldingStrategy.UpdateFoldings(foldingManager, editor.Document);
            }
        }
        #endregion
    }
}
