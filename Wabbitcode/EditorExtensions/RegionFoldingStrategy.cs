using System;
using System.Collections.Generic;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.Properties;

namespace Revsoft.Wabbitcode.EditorExtensions
{
	public class RegionFoldingStrategy : IFoldingStrategy
	{
		/// <summary>
		/// Generates the foldings for our document.
		/// </summary>
		/// <param name="document">The current document.</param>
		/// <param name="fileName">The filename of the document.</param>
		/// <param name="parseInformation">Extra parse information, not used in this sample.</param>
		/// <returns>A list of FoldMarkers.</returns>
		public List<FoldMarker> GenerateFoldMarkers(IDocument document, string fileName, object parseInformation)
		{
			List<FoldMarker> list = new List<FoldMarker>();

			Stack<FoldingItem> regionLines = new Stack<FoldingItem>();
			Stack<FoldingItem> ifLines = new Stack<FoldingItem>();
			Stack<FoldingItem> macroLines = new Stack<FoldingItem>();
			Stack<int> commentLines = new Stack<int>();
			// Create foldmarkers for the whole document, enumerate through every line.
			for (int i = 0; i < document.TotalNumberOfLines; i++)
			{
				var seg = document.GetLineSegment(i);
				int offs, end = document.TextLength;
				char c;
				for (offs = seg.Offset; offs < end && ((c = document.GetCharAt(offs)) == ' ' || c == '\t'); offs++)
				{ }
				if (offs == end)
					break;
				int spaceCount = offs - seg.Offset;

				// now offs points to the first non-whitespace char on the line
				if (
					document.GetCharAt(offs) != '#')
					continue;
				var options = Settings.Default.caseSensitive ? StringComparison.Ordinal : StringComparison.OrdinalIgnoreCase;
				string normalText = document.GetText(offs, seg.Length - spaceCount);
				if (normalText.StartsWith("#region", options))
				{
					regionLines.Push(new FoldingItem(i, normalText.Remove(0, "#region".Length).Trim()));
				}

				if (normalText.StartsWith("#ifdef", options) || normalText.StartsWith("#if", options) ||
				    normalText.StartsWith("#ifndef", options))
				{
					ifLines.Push(new FoldingItem(i, normalText));
				}

				if (normalText.StartsWith("#macro", options))
				{
					int paren = normalText.IndexOf('(');
					if (paren == -1)
					{
						paren = normalText.Length - 1;
					}

					string substring = normalText.Substring(0, paren);
					macroLines.Push(new FoldingItem(i, substring));
				}
				if (normalText.StartsWith("#comment", options))
				{
					commentLines.Push(i);
				}

				if (normalText.StartsWith("#endregion", options) && regionLines.Count > 0)
				{
					// Add a new FoldMarker to the list.
					FoldingItem start = regionLines.Pop();
					list.Add(new FoldMarker(document, start.Offset, 0, i, spaceCount + "#endregion".Length, FoldType.Region, start.Text));
				}

				if (normalText.StartsWith("#else", options) && ifLines.Count > 0)
				{
					// Add a new FoldMarker to the list.
					FoldingItem start = ifLines.Pop();
					list.Add(new FoldMarker(document, start.Offset, 0, i - 1, document.GetLineSegment(i - 1).Length, FoldType.TypeBody, start.Text));
					ifLines.Push(new FoldingItem(i, "#else"));
				}

				if (normalText.StartsWith("#endif", options) && ifLines.Count > 0)
				{
					// Add a new FoldMarker to the list.
					FoldingItem start = ifLines.Pop();
					list.Add(new FoldMarker(document, start.Offset, 0 /*document.GetLineSegment(start).Length*/,
						i, spaceCount + "#endif".Length, FoldType.TypeBody, start.Text));
				}

				if (normalText.StartsWith("#endmacro", options) && macroLines.Count > 0)
				{
					// Add a new FoldMarker to the list.
					FoldingItem start = macroLines.Pop();
					list.Add(new FoldMarker(document, start.Offset, 0,
						i, spaceCount + "#endmacro".Length, FoldType.MemberBody, start.Text));
				}

				if (normalText.StartsWith("#endcomment", options) && commentLines.Count > 0)
				{
					// Add a new FoldMarker to the list.
					int start = commentLines.Pop();
					list.Add(new FoldMarker(document, start, 0 /*document.GetLineSegment(start).Length*/,
						i, spaceCount + "#endcomment".Length, FoldType.Region, "#comment"));
				}
			}
			return list;
		}
	}
}