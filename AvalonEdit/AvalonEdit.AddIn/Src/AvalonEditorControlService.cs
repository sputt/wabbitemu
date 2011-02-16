// Copyright (c) AlphaSierraPapa for the SharpDevelop Team (for details please see \doc\copyright.txt)
// This code is distributed under the GNU LGPL (for details please see \doc\license.txt)

using System;

namespace Revsoft.Wabbitcode.AvalonEditExtension
{
	/// <summary>
	/// Implementation of IEditorControlService, allows other addins to create editors or access the options without
	/// requiring a reference to AvalonEdit.AddIn.
	/// </summary>
	public class AvalonEditorControlService : IEditorControlService
	{
		public ITextEditorOptions GlobalOptions {
			get { return CodeEditorOptions.Instance; }
		}
		
		public ITextEditor CreateEditor(out object control)
		{
			WabbitcodeTextEditor editor = new WabbitcodeTextEditor();
			control = editor;
			return new CodeCompletionEditorAdapter(editor);
		}
	}
}
