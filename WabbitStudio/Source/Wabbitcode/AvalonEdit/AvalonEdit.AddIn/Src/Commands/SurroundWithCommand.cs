// Copyright (c) AlphaSierraPapa for the SharpDevelop Team (for details please see \doc\copyright.txt)
// This code is distributed under the GNU LGPL (for details please see \doc\license.txt)

using System;
using System.IO;
using System.Linq;

using Revsoft.Wabbitcode.AvalonEditExtension.Snippets;
using ICSharpCode.AvalonEdit.Editing;

namespace Revsoft.Wabbitcode.AvalonEditExtension.Commands
{
	public class SurroundWithCommand : AbstractMenuCommand
	{
		/// <summary>
		/// Starts the command
		/// </summary>
		public override void Run()
		{
			ICodeEditorProvider provider = WorkbenchSingleton.Workbench.ActiveViewContent as ICodeEditorProvider;
			
			if (provider == null)
				return;
			
			CodeSnippetGroup group = SnippetManager.Instance.FindGroup(Path.GetExtension(provider.TextEditor.FileName));
			
			if (group == null)
				return;
			
			DefaultCompletionItemList list = new DefaultCompletionItemList();
			
			list.Items.AddRange(group.Snippets.Where(i => i.HasSelection).Select(item => item.CreateCompletionItem(provider.TextEditor)));
			
			new CodeSnippetCompletionWindow(provider.TextEditor, list).Show();
		}
	}
}
