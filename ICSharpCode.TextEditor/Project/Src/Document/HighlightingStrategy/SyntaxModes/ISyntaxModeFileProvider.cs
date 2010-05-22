// <file>
//     <copyright see="prj:///doc/copyright.txt"/>
//     <license see="prj:///doc/license.txt"/>
//     <owner name="Mike Krüger" email="mike@Revsoft.net"/>
//     <version>$Revision: 1301 $</version>
// </file>

using System;
using System.Collections.Generic;
using System.Xml;

namespace Revsoft.TextEditor.Document
{
	public interface ISyntaxModeFileProvider
	{
		ICollection<SyntaxMode> SyntaxModes {
			get;
		}
		
		XmlTextReader GetSyntaxModeFile(SyntaxMode syntaxMode);
		void UpdateSyntaxModeList();
	}
}
