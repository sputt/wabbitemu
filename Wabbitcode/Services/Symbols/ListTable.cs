using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Symbols
{
	public class ListTable
	{
		private Dictionary<CalcLocation, DocumentLocation> _calcToFile;
		private Dictionary<DocumentLocation, CalcLocation> _fileToCalc;

		public ListTable()
		{
			_calcToFile = new Dictionary<CalcLocation, DocumentLocation>();
			_fileToCalc = new Dictionary<DocumentLocation, CalcLocation>();
		}

		public void ParseListFile(string listFileContents)
		{
            var calcToFile = new Dictionary<CalcLocation, DocumentLocation>(700000);
            var fileToCalc = new Dictionary<DocumentLocation, CalcLocation>(700000);
		    StreamWriter writer = null;
            Regex fileRegex = new Regex("Listing for (built\\-in|file) (\"(?<file>.+)\"|(?<file>.+))", RegexOptions.Compiled);
            Regex listingRegex = new Regex(@"(?<lineNum>[\d| ]{5}) (?<page>[0-9A-F]{2}):(?<addr>[0-9A-F]{4}) (?<byte1>[0-9A-F]{2}|- |  ) ([0-9A-F]{2}|- |  ) ([0-9A-F]{2}|- |  ) ([0-9A-F]{2}|- |  ) (?<line>.*)", RegexOptions.Compiled);
			string currentFile = string.Empty;
			string[] lines = listFileContents.Split('\n');
            Dictionary<string, int> fcreateForFile = new Dictionary<string, int>();
			foreach (string line in lines.Where(line => !string.IsNullOrWhiteSpace(line)))
			{
			    Match fileMatch = fileRegex.Match(line);
			    Match listingMatch = listingRegex.Match(line);
				if (fileMatch.Success)
				{
				    string file = fileMatch.Groups["file"].Value.Trim();
                    if (file == "fcreate")
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
						currentFile = currentFile + fcreateNum + ".fcreate";
                        writer = new StreamWriter(currentFile);
                    }
					else
                    {
                        currentFile = file;
                        if (writer != null)
                        {
                            writer.Flush();
                            writer.Close();
                        }
                        writer = null;
                    }
				} 
                else if (listingMatch.Success)
                {
			        string stringLineNumber = listingMatch.Groups["lineNum"].Value.Trim();
                    int lineNumber = Convert.ToInt32(stringLineNumber);
                    ushort address = ushort.Parse(listingMatch.Groups["addr"].Value, NumberStyles.HexNumber);
                    byte page = byte.Parse(listingMatch.Groups["page"].Value, NumberStyles.HexNumber);

                    // correction for ram pages
                    if (page == 0 && address >= 0x8000 && address < 0xC000)
                    {
                        page = 1;
                    }

                    if (writer != null)
                    {
                        writer.WriteLine(listingMatch.Groups["line"].Value.TrimEnd());
                    }

                    if (listingMatch.Groups["byte1"].Value.Contains("-") || string.IsNullOrWhiteSpace(listingMatch.Groups["byte1"].Value))
                    {
                        continue;
                    }

                    DocumentLocation key = new DocumentLocation(currentFile.ToLower(), lineNumber);
                    CalcLocation value = new CalcLocation(address, page, address >= 0x8000);

                    if (!calcToFile.ContainsKey(value))
                    {
                        calcToFile.Add(value, key);
                    }
                    if (!fileToCalc.ContainsKey(key))
                    {
                        fileToCalc.Add(key, value);
                    }
                }
			}

            if (writer != null)
            {
                writer.Flush();
                writer.Close();
            }

		    _fileToCalc = fileToCalc;
		    _calcToFile = calcToFile;
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
		    if (string.IsNullOrEmpty(filename))
		    {
		        return null;
		    }

			CalcLocation value;
			DocumentLocation key = new DocumentLocation(filename.ToLower(), lineNumber);
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

		    if (!smallerListings.Any())
		    {
                // no listing in this file
		        return null;
		    }

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
	}
}
