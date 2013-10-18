using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Symbols
{
	public class ListTable
	{
		private readonly Dictionary<CalcLocation, DocumentLocation> _calcToFile;
		private readonly Dictionary<DocumentLocation, CalcLocation> _fileToCalc;

		public ListTable()
		{
			_calcToFile = new Dictionary<CalcLocation, DocumentLocation>(700000);
			_fileToCalc = new Dictionary<DocumentLocation, CalcLocation>(700000);
		}

		public void ParseListFile(string listFileContents, string projectPath)
		{
			string currentFile = string.Empty;
			string[] lines = listFileContents.Split('\n');
			Dictionary<string, int> fcreateForFile = new Dictionary<string, int>();
			foreach (string line in lines.Where(line => !string.IsNullOrWhiteSpace(line)))
			{
				if (line.Contains("Listing for"))
				{
					if (line.Trim() == "Listing for built-in fcreate")
					{
						int fcreateNum;
						if (!fcreateForFile.TryGetValue(currentFile, out fcreateNum))
						{
							fcreateForFile[currentFile] = 0;
						}
						else
						{
							fcreateForFile[currentFile] = ++fcreateNum;
						}
						
						currentFile = currentFile + "fcreate" + fcreateNum;
					}
					else
					{
						currentFile = Path.Combine(projectPath, line.Substring(line.IndexOf('\"') + 1,
							line.LastIndexOf('\"') - line.IndexOf('\"') - 1)).ToLower();
					}
				}

				if (line.Substring(0, 5) != "     " && line.Substring(13, 12) != "            " &&
				    line.Substring(13, 12) != " -  -  -  - " && !line.Contains("Listing for"))
				{
					string stringLineNumber = line.Substring(0, 5).Trim();
					int lineNumber = Convert.ToInt32(stringLineNumber);
					string temp = line.Substring(6, 7);
					ushort address = ushort.Parse(temp.Substring(3, 4), NumberStyles.HexNumber);
					byte page = byte.Parse(temp.Substring(0, 2), NumberStyles.HexNumber);

					// correction for ram pages
					if (page == 0 && address >= 0x8000 && address < 0xC000)
					{
						page = 1;
					}
					DocumentLocation key = new DocumentLocation(currentFile, lineNumber);
					CalcLocation value = new CalcLocation(address, page, address >= 0x8000);

					if (!_calcToFile.ContainsKey(value))
					{
						_calcToFile.Add(value, key);
					}
					if (!_fileToCalc.ContainsKey(key))
					{
						_fileToCalc.Add(key, value);
					}

				}
			}
		}

		/// <summary>
		/// Maps a filename and line number to a page and address
		/// </summary>
		/// <param name="filename">The absolute path of the file</param>
		/// <param name="lineNumber">The 1-indexed line number</param>
		/// <returns>The absolute location on the calculator the file and line number map to. Returns null 
		/// if the file line number combination do not map to a location on the calc</returns>
		public CalcLocation GetCalcLocation(string filename, int lineNumber)
		{
			CalcLocation value;
			DocumentLocation key = new DocumentLocation(filename.ToLower(), lineNumber + 1);
			_fileToCalc.TryGetValue(key, out value);
			return value;
		}

		public DocumentLocation GetFileLocation(int page, int address, bool isRam)
		{
			CalcLocation value = new CalcLocation((ushort)address, (byte)page, isRam);
			DocumentLocation key;
			_calcToFile.TryGetValue(value, out key);
			return key;
		}

		/// <summary>
		/// Maps a filename and line number to a page and address greater than the GetCalcLocation mapping
		/// </summary>
		/// <param name="fileName">The absolute path of the file</param>
		/// <param name="lineNumber">The 1-indexed line number</param>
		/// <returns>The absolute location on the calculator the file and line number map to. If
		/// it does not map absolutely, it returns the next closest location. Returns null if
		/// the file never maps to a location on calc</returns>
		public CalcLocation GetNextNearestCalcLocation(string fileName, int lineNumber)
		{
			var initialListing = _fileToCalc.Where(s => string.Compare(s.Key.FileName, fileName, true) == 0);
			// we shouldn't ever call this when we are adding more, but just to be sure,
			// we'll avoid multiple enumeration
			var listInFile = initialListing as KeyValuePair<DocumentLocation, CalcLocation>[] ?? initialListing.ToArray();
			var smallerListings = listInFile.Where(s => s.Key.LineNumber < lineNumber).ToArray();
			var largerListings = listInFile.Where(s => s.Key.LineNumber >= lineNumber).ToArray();
			if (largerListings.Any())
			{
				int min = largerListings.Min(s => s.Key.LineNumber);
				return largerListings.Single(s => s.Key.LineNumber == min).Value;
			}
			else if (smallerListings.Any())
			{
				int max = smallerListings.Max(s => s.Key.LineNumber);
				CalcLocation smallerLocation = smallerListings.Single(s => s.Key.LineNumber == max).Value;
				ushort address = smallerLocation.Address;
				do
				{
					address++;
					if ((address % 0x4000) == 0)
					{
						// crossing page boundaries means we have no clue where its going
						return null;
					}
				} while (GetFileLocation(smallerLocation.Page, address, smallerLocation.IsRam) == null);
				return new CalcLocation(address, smallerLocation.Page, smallerLocation.IsRam);
			}
			else
			{
				// no listing in this file
				return null;
			}
		}
	}
}
