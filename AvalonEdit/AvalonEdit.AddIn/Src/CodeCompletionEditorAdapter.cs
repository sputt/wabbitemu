// Copyright (c) AlphaSierraPapa for the SharpDevelop Team (for details please see \doc\copyright.txt)
// This code is distributed under the GNU LGPL (for details please see \doc\license.txt)

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using Revsoft.Wabbitcode.AvalonEditExtension.Snippets;
using Revsoft.Wabbitcode.AvalonEditExtension.CodeCompletion;
using Revsoft.Wabbitcode.AvalonEditExtension.Src;
using Revsoft.Wabbitcode.AvalonEditExtension.Interface;

namespace Revsoft.Wabbitcode.AvalonEditExtension
{
	class CodeCompletionEditorAdapter : AvalonEditTextEditorAdapter
	{
		WabbitcodeTextEditor textEditor;
		
		public CodeCompletionEditorAdapter(WabbitcodeTextEditor textEditor) : base(textEditor)
		{
			this.textEditor = textEditor;
		}
		
		public override ICompletionListWindow ShowCompletionWindow(ICompletionItemList data)
		{
			if (data == null || !data.Items.Any())
				return null;
			var window = new WabbitcodeCompletionWindow(this, textEditor.TextArea, data);
			textEditor.ShowCompletionWindow(window);
			return window;
		}
		
		public override IInsightWindow ShowInsightWindow(IEnumerable<IInsightItem> items)
		{
			if (items == null)
				return null;
			var insightWindow = new WabbitcodeInsightWindow(this.TextEditor.TextArea);
			insightWindow.Items.AddRange(items);
			if (insightWindow.Items.Count > 0) {
				insightWindow.SelectedItem = insightWindow.Items[0];
			} else {
				// don't open insight window when there are no items
				return null;
			}
			textEditor.ShowInsightWindow(insightWindow);
			return insightWindow;
		}
		
		public override IInsightWindow ActiveInsightWindow {
			get { return textEditor.ActiveInsightWindow; }
		}
		
		public override ICompletionListWindow ActiveCompletionWindow {
			get { return textEditor.ActiveCompletionWindow; }
		}
		
		public override ITextEditorOptions Options {
			get { return CodeEditorOptions.Instance; }
		}
	}
}
