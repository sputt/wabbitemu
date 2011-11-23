using System;
using System.IO;
using System.Linq;
using System.Windows;
using System.Windows.Threading;
using AvalonDock;
using ICSharpCode.AvalonEdit.Folding;
using ICSharpCode.AvalonEdit.Highlighting;
using ICSharpCode.AvalonEdit.Indentation;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Utilities;

namespace Revsoft.Wabbitcode.Panels
{
	/// <summary>
	/// Interaction logic for Editor.xaml
	/// </summary>
	public partial class Editor : DocumentContent, IWabbitcodePanel, IEditor
	{
		public FilePath FilePath { get; private set; }

		IDockingService dockingService;
		IDocumentService documentService;

		FoldingManager foldingManager;
		AbstractFoldingStrategy foldingStrategy;

		public Editor()
		{
			InitializeComponent();
			editor.TextArea.Caret.PositionChanged += Caret_PositionChanged;
			editor.Document.UndoStack.PropertyChanged += UndoStack_PropertyChanged;

			foldingManager = FoldingManager.Install(editor.TextArea);
			foldingStrategy = new AsmFoldingStrategy();

			documentService = ServiceFactory.Instance.GetServiceInstance<DocumentService>();
			dockingService = ServiceFactory.Instance.GetServiceInstance<DockingService>();
		}

		#region Layout
		const string fileNameAttribute = "FileName";
		public override void RestoreLayout(System.Xml.XmlElement contentElement)
		{
			string fileName = contentElement.GetAttribute(fileNameAttribute);
			documentService.OpenDocument(this, fileName);
			base.RestoreLayout(contentElement);
		}

		public override void SaveLayout(System.Xml.XmlWriter storeWriter)
		{
			storeWriter.WriteAttributeString(fileNameAttribute, FilePath);
			base.SaveLayout(storeWriter);
		}
		#endregion

		#region IUndoable Functions
		public void Undo()
		{
			editor.Undo();
		}

		public void Redo()
		{
			editor.Redo();
		}
		#endregion

		#region IClipboard
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
		#endregion

		#region IEditor
		public void SelectAll()
		{
			editor.SelectAll();
		}

		public void OpenFile(string filename)
		{
			OpenFile(new FilePath(filename));
		}

		public void OpenFile(FilePath filename)
		{
			FilePath = filename;
			editor.Load(filename);
			editor.SyntaxHighlighting = HighlightingManager.Instance.GetDefinitionByExtension(Path.GetExtension(filename));
		}

		public void SaveFile()
		{
			editor.Save(FilePath);
			editor.Document.UndoStack.MarkAsOriginalFile();
			editor.SyntaxHighlighting = HighlightingManager.Instance.GetDefinitionByExtension(Path.GetExtension(FilePath));
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
			int col = 1 + editor.Document.GetText(lineOffset, chOffset - 1)
					.Sum(ch => ch == '\t' ? Properties.Settings.Default.TabSize : 1);
			dockingService.StatusBar.SetCaretPosition(col, line, chOffset);
		}

		void UndoStack_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "IsOriginalFile":
					this.Title = Path.GetFileName(FilePath);
					if (!editor.Document.UndoStack.IsOriginalFile)
					{
						this.Title += "*";
					}
					break;
			}
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

		private void EditorDocument_GotFocus(object sender, RoutedEventArgs e)
		{
			
		}
		#endregion
	}
}
