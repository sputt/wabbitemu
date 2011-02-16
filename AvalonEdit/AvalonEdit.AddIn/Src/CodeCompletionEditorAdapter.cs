// Copyright (c) AlphaSierraPapa for the SharpDevelop Team (for details please see \doc\copyright.txt)
// This code is distributed under the GNU LGPL (for details please see \doc\license.txt)

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using Revsoft.Wabbitcode.AvalonEditExtension.Options;
using Revsoft.Wabbitcode.AvalonEditExtension.Snippets;
using Revsoft.Wabbitcode.Indentation;
using Revsoft.Core;
using Revsoft.SharpDevelop;
using Revsoft.SharpDevelop.Editor;
using Revsoft.SharpDevelop.Editor.AvalonEdit;
using Revsoft.SharpDevelop.Editor.CodeCompletion;

namespace Revsoft.Wabbitcode.AvalonEditExtension
{
	class CodeCompletionEditorAdapter : AvalonEditTextEditorAdapter
	{
		WabbitcodeTextEditor textEditor;
		
		public CodeCompletionEditorAdapter(WabbitcodeTextEditor textEditor)
			: base(textEditor)
		{
			this.textEditor = textEditor;
		}
		
		public override ICompletionListWindow ShowCompletionWindow(ICompletionItemList data)
		{
			if (data == null || !data.Items.Any())
				return null;
			WabbitcodeCompletionWindow window = new WabbitcodeCompletionWindow(this, this.TextEditor.TextArea, data);
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
