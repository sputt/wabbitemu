// Copyright (c) AlphaSierraPapa for the SharpDevelop Team (for details please see \doc\copyright.txt)
// This code is distributed under the GNU LGPL (for details please see \doc\license.txt)

using System;
using System.Linq;
using ICSharpCode.AvalonEdit.Folding;
using Revsoft.Wabbitcode.AvalonEditExtension.Interface;

namespace Revsoft.Wabbitcode.AvalonEditExtension.Commands
{
	/*public static class FoldingCommands
	{
		public static void ToggleFolding()
		{
			ITextEditorProvider provider = WorkbenchSingleton.Workbench.ActiveViewContent as ITextEditorProvider;
			ITextEditor editor = provider.TextEditor;
			ParserFoldingStrategy strategy = editor.GetService(typeof(ParserFoldingStrategy)) as ParserFoldingStrategy;
			
			if (strategy != null) {
				// look for folding on this line:
				FoldingSection folding = strategy.FoldingManager.GetNextFolding(editor.Document.PositionToOffset(editor.Caret.Line, 1));
				if (folding == null || editor.Document.GetLineForOffset(folding.StartOffset).LineNumber != editor.Caret.Line) {
					// no folding found on current line: find innermost folding containing the caret
					folding = strategy.FoldingManager.GetFoldingsContaining(editor.Caret.Offset).LastOrDefault();
				}
				if (folding != null) {
					folding.IsFolded = !folding.IsFolded;
				}
			}
		}

		public override void ToggleAllFoldings()
		{
			ITextEditorProvider provider = WorkbenchSingleton.Workbench.ActiveViewContent as ITextEditorProvider;
			ITextEditor editor = provider.TextEditor;
			ParserFoldingStrategy strategy = editor.GetService(typeof(ParserFoldingStrategy)) as ParserFoldingStrategy;
			
			if (strategy != null) {
				bool doFold = true;
				foreach (FoldingSection fm in strategy.FoldingManager.AllFoldings) {
					if (fm.IsFolded) {
						doFold = false;
						break;
					}
				}
				foreach (FoldingSection fm in strategy.FoldingManager.AllFoldings) {
					fm.IsFolded = doFold;
				}
			}
		}
	
		public override void ShowDefinitionsOnly()
		{
			ITextEditorProvider provider = WorkbenchSingleton.Workbench.ActiveViewContent as ITextEditorProvider;
			ITextEditor editor = provider.TextEditor;
			ParserFoldingStrategy strategy = editor.GetService(typeof(ParserFoldingStrategy)) as ParserFoldingStrategy;
			
			if (strategy != null) {
				foreach (FoldingSection fm in strategy.FoldingManager.AllFoldings) {
					fm.IsFolded = ParserFoldingStrategy.IsDefinition(fm);
				}
			}
		}
	}*/
}
