using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ICSharpCode.AvalonEdit.Folding;
using ICSharpCode.AvalonEdit.Document;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode
{
	/// <summary>
	/// Allows producing foldings from a document based on braces.
	/// </summary>
	public class AsmFoldingStrategy : AbstractFoldingStrategy
	{
		/// <summary>
		/// Gets/Sets the opening brace. The default value is '{'.
		/// </summary>
		public char OpeningBrace { get; set; }
		
		/// <summary>
		/// Gets/Sets the closing brace. The default value is '}'.
		/// </summary>
		public char ClosingBrace { get; set; }
		
		/// <summary>
		/// Creates a new BraceFoldingStrategy.
		/// </summary>
		public AsmFoldingStrategy()
		{
			this.OpeningBrace = '{';
			this.ClosingBrace = '}';
		}
		
		/// <summary>
		/// Create <see cref="NewFolding"/>s for the specified document.
		/// </summary>
		public override IEnumerable<NewFolding> CreateNewFoldings(TextDocument document, out int firstErrorOffset)
		{
			firstErrorOffset = -1;
			return CreateNewFoldings(document);
		}
		
		/// <summary>
		/// Create <see cref="NewFolding"/>s for the specified document.
		/// </summary>
		public static IEnumerable<NewFolding> CreateNewFoldings(TextDocument document)
		{
			List<NewFolding> list = new List<NewFolding>();

			Stack<FoldingItem> regionLines = new Stack<FoldingItem>();
			Stack<FoldingItem> ifLines = new Stack<FoldingItem>();
			Stack<FoldingItem> macroLines = new Stack<FoldingItem>();
			Stack<int> commentLines = new Stack<int>();
			int end = document.TextLength;
			// Create NewFoldings for the whole document, enumerate through every line.
			for (int i = 1; i <= document.LineCount; i++)
			{
				var seg = document.GetLineByNumber(i);
				int offs, lineStart;
				char c;
				for (offs = lineStart = seg.Offset; offs < end && ((c = document.GetCharAt(offs)) == ' ' || c == '\t'); offs++) ;
				if (offs == end)
					break;
				int spaceCount = offs - seg.Offset;

				// now offs points to the first non-whitespace char on the line
				if (document.GetCharAt(offs) != '#') 
					continue;
				string normalText = document.GetText(offs, seg.Length - spaceCount);
				string text = normalText.ToLower();
				if (text.StartsWith(ParserService.regionString))
				{
					regionLines.Push(new FoldingItem(offs, normalText.Remove(0, ParserService.regionString.Length).Trim()));
				}
				else if (text.StartsWith(ParserService.ifdefString) ||
						text.StartsWith(ParserService.ifString) ||
						text.StartsWith(ParserService.ifndefString))
				{
					ifLines.Push(new FoldingItem(offs, normalText));
				}
				else if (text.StartsWith(ParserService.macroString))
				{
					int paren = text.IndexOf('(');
					if (paren == -1)
						paren = text.Length;
					string substring = normalText.Substring(0, paren);
					macroLines.Push(new FoldingItem(offs, substring));
				}
				else if (text.StartsWith(ParserService.commentString))
				{
					commentLines.Push(lineStart);
				}
				else if (text.StartsWith(ParserService.elseString) && ifLines.Count > 0)
				{
					// Add a new NewFolding to the list.
					FoldingItem start = ifLines.Pop();
					list.Add(new NewFolding(start.Offset, lineStart - 2, start.Text));
					ifLines.Push(new FoldingItem(offs, ParserService.elseString));
				}
				else if (text.StartsWith("#end"))
				{
					if (text.StartsWith(ParserService.endRegionString) && regionLines.Count > 0)
					{
						// Add a new NewFolding to the list.
						FoldingItem start = regionLines.Pop();
						list.Add(new NewFolding(start.Offset, offs + ParserService.endRegionString.Length,
								string.IsNullOrEmpty(start.Text) ? ParserService.regionString : start.Text));
					}
					else if (text.StartsWith(ParserService.endIfString) && ifLines.Count > 0)
					{
						// Add a new NewFolding to the list.
						FoldingItem start = ifLines.Pop();
						list.Add(new NewFolding(start.Offset, offs + ParserService.endIfString.Length, start.Text));
					}
					else if (text.StartsWith(ParserService.endMacroString) && macroLines.Count > 0)
					{
						// Add a new NewFolding to the list.
						FoldingItem start = macroLines.Pop();
						list.Add(new NewFolding(start.Offset, offs + ParserService.endMacroString.Length, start.Text));
					}
					else if (text.StartsWith(ParserService.endCommentString) && commentLines.Count > 0)
					{
						// Add a new NewFolding to the list.
						int start = commentLines.Pop();
						list.Add(new NewFolding(start, offs + ParserService.endCommentString.Length, ParserService.commentString));
					}
				}
			}
			var sorted = from folding in list orderby folding.StartOffset ascending select folding;
			return sorted;
		}
	}

	public class FoldingItem
	{
		int offset;
		public int Offset
		{
			get { return offset; }
		}
		string text;
		public string Text
		{
			get { return text; }
		}
		public FoldingItem(int offset, string text)
		{
			this.offset = offset;
			this.text = text;
		}
	}
}
