// <file>
//     <copyright see="prj:///doc/copyright.txt"/>
//     <license see="prj:///doc/license.txt"/>
//     <owner name="Mike Krüger" email="mike@Revsoft.net"/>
//     <version>$Revision: 915 $</version>
// </file>

using System;
using System.Collections.Generic;
using System.Xml;

namespace Revsoft.TextEditor.Document
{
	/// <summary>
	/// This class is used for storing the state of a breakpoint manager 
	/// </summary>
	public class BreakpointManagerMemento
	{
		List<int> breakpoints = new List<int>();
		
		/// <value>
		/// Contains all breakpoints as int values
		/// </value>
		public List<int> Breakpoints {
			get {
				return breakpoints;
			}
			set {
				breakpoints = value;
			}
		}
		
		/// <summary>
		/// Validates all breakpoints if they're in range of the document.
		/// (removing all breakpoints &lt; 0 and breakpoints &gt; max. line number
		/// </summary>
		public void CheckMemento(IDocument document)
		{
			for (int i = 0; i < breakpoints.Count; ++i) {
				int mark = (int)breakpoints[i];
				if (mark < 0 || mark >= document.TotalNumberOfLines) {
					breakpoints.RemoveAt(i);
					--i;
				}
			}
		}
		
		/// <summary>
		/// Creates a new instance of <see cref="BreakpointManagerMemento"/>
		/// </summary>
		public BreakpointManagerMemento()
		{
		}
		
		/// <summary>
		/// Creates a new instance of <see cref="BreakpointManagerMemento"/>
		/// </summary>
		public BreakpointManagerMemento(XmlElement element)
		{
			foreach (XmlElement el in element.ChildNodes) {
				breakpoints.Add(Int32.Parse(el.Attributes["line"].InnerText));
			}
		}
		
		/// <summary>
		/// Creates a new instance of <see cref="BreakpointManagerMemento"/>
		/// </summary>
		public BreakpointManagerMemento(List<int> breakpoints)
		{
			this.breakpoints = breakpoints;
		}
		
		/// <summary>
		/// Converts a xml element to a <see cref="BreakpointManagerMemento"/> object
		/// </summary>
		public object FromXmlElement(XmlElement element)
		{
			return new BreakpointManagerMemento(element);
		}
		
		/// <summary>
		/// Converts this <see cref="BreakpointManagerMemento"/> to a xml element
		/// </summary>
		public XmlElement ToXmlElement(XmlDocument doc)
		{
			XmlElement breakpointnode  = doc.CreateElement("Breakpoints");
			
			foreach (int line in breakpoints) {
				XmlElement markNode = doc.CreateElement("Mark");
				
				XmlAttribute lineAttr = doc.CreateAttribute("line");
				lineAttr.InnerText = line.ToString();
				markNode.Attributes.Append(lineAttr);
						
				breakpointnode.AppendChild(markNode);
			}
			
			return breakpointnode;
		}
	}
}
