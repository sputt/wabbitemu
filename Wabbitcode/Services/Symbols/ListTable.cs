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
        private static readonly Regex CallRegex = new Regex(@"\s*(?<command>\w*call\w*)[\(?|\s]\s*((?<condition>z|nz|c|nc),\s*)?(?<call>\w*?)\)?\s*(;.*)?$",
                RegexOptions.Compiled | RegexOptions.IgnoreCase);
        private static readonly Regex FileRegex = new Regex("Listing for (built\\-in|file) (\"(?<file>.+)\"|(?<file>.+))", RegexOptions.Compiled);
        private static readonly Regex ListingRegex = new Regex(@"(?<lineNum>[\d| ]{5}) (?<page>[0-9A-F]{2}):(?<addr>[0-9A-F]{4}) (?<byte1>[0-9A-F]{2}|- |  ) (?<byte2>[0-9A-F]{2}|- |  ) (?<byte3>[0-9A-F]{2}|- |  ) (?<byte4>[0-9A-F]{2}|- |  ) (?<line>.*)", RegexOptions.Compiled);
        private static readonly Regex ListingContRegex = new Regex(@"\s{14}(?<byte1>[0-9A-F]{2}|- |  ) (?<byte2>[0-9A-F]{2}|- |  ) (?<byte3>[0-9A-F]{2}|- |  ) (?<byte4>[0-9A-F]{2}|- |  ) (?<line>.*)");

        private Dictionary<CalcLocation, DocumentLocation> _calcToFile;
        private Dictionary<DocumentLocation, CalcLocation> _fileToCalc;
        private Dictionary<CalcLocation, CallerInformation> _callerInformation;

        public ListTable()
        {
            _calcToFile = new Dictionary<CalcLocation, DocumentLocation>();
            _fileToCalc = new Dictionary<DocumentLocation, CalcLocation>();
        }

        public void ParseListFile(string listFileContents)
        {
            var calcToFile = new Dictionary<CalcLocation, DocumentLocation>(700000);
            var fileToCalc = new Dictionary<DocumentLocation, CalcLocation>(700000);
            var callerInfo = new Dictionary<CalcLocation, CallerInformation>();
            StreamWriter writer = null;
            FilePath currentFile = new FilePath(string.Empty);
            var lines = listFileContents.Split('\n');
            var fcreateForFile = new Dictionary<FilePath, int>();
            CallerInformation callInfoToAdd = null;

            for (int i = 0; i < lines.Length; i++)
            {
                var line = lines[i];
                if (string.IsNullOrWhiteSpace(line))
                {
                    continue;
                }

                Match fileMatch = FileRegex.Match(line);
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
                        currentFile = new FilePath(currentFile + fcreateNum + ".fcreate");
                        writer = new StreamWriter(currentFile);
                    }
                    else
                    {
                        currentFile = new FilePath(file);
                        if (writer != null)
                        {
                            writer.Flush();
                            writer.Close();
                        }
                        writer = null;
                    }
                }
                else
                {
                    Match listingMatch = ListingRegex.Match(line);
                    if (!listingMatch.Success)
                    {
                        continue;
                    }

                    string stringLineNumber = listingMatch.Groups["lineNum"].Value.Trim();
                    int lineNumber = int.Parse(stringLineNumber);
                    ushort address = ushort.Parse(listingMatch.Groups["addr"].Value, NumberStyles.HexNumber);
                    byte page = byte.Parse(listingMatch.Groups["page"].Value, NumberStyles.HexNumber);
                    string codeLine = listingMatch.Groups["line"].Value;

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

                    DocumentLocation key = new DocumentLocation(currentFile, lineNumber);
                    CalcLocation value = new CalcLocation(address, page, address >= 0x8000);

                    do
                    {
                        line = lines[i + 1];
                        listingMatch = ListingContRegex.Match(line);
                        if (listingMatch.Success)
                        {
                            i++;
                            codeLine = listingMatch.Groups["line"].Value;
                        }
                    } while (listingMatch.Success);

                    if (callInfoToAdd != null)
                    {
                        callerInfo.Add(value, callInfoToAdd);
                    }

                    callInfoToAdd = ParseCallLine(codeLine, key);

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
            _callerInformation = callerInfo;
        }

        private CallerInformation ParseCallLine(string codeLine, DocumentLocation location)
        {
            Match match = CallRegex.Match(codeLine);
            if (!match.Success)
            {
                return null;
            }

            return new CallerInformation(match.Groups["command"].Value, match.Groups["condition"].Value, match.Groups["call"].Value, location);
        }

        public CallerInformation CheckValidCall(ushort callerLocation, bool isCallerInRam, byte calleePage)
        {
            CalcLocation location = new CalcLocation(callerLocation, calleePage, isCallerInRam);
            CallerInformation info = GetCallInfo(location);
            if (isCallerInRam || info != null)
            {
                return info;
            }

            for (byte page = 0; page < 0x20; page++)
            {
                if (page == calleePage)
                {
                    continue;
                }

                location = new CalcLocation(callerLocation, page, false);
                info = GetCallInfo(location);
                if (info != null)
                {
                    return info;
                }
            }

            return null;
        }

        private CallerInformation GetCallInfo(CalcLocation location)
        {
            CallerInformation info;
            _callerInformation.TryGetValue(location, out info);
            return info;
        }

        /// <summary>
        /// Maps a filename and line number to a page and address
        /// </summary>
        /// <param name="filename">The absolute path of the file</param>
        /// <param name="lineNumber">The 1-indexed line number</param>
        /// <returns>The absolute location on the calculator the file and line number map to. Returns null 
        /// if the file line number combination do not map to a location on the calc</returns>
        public CalcLocation GetCalcLocation(FilePath filename, int lineNumber)
        {
            if (string.IsNullOrEmpty(filename))
            {
                return null;
            }

            CalcLocation value;
            DocumentLocation key = new DocumentLocation(filename, lineNumber);
            _fileToCalc.TryGetValue(key, out value);
            return value;
        }

        public DocumentLocation GetFileLocation(int page, int address, bool isRam)
        {
            CalcLocation value = new CalcLocation((ushort) address, (byte) page, isRam);
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
        public CalcLocation GetNextNearestCalcLocation(FilePath fileName, int lineNumber)
        {
            var initialListing = _fileToCalc.Where(s => s.Key.FileName == fileName);
            // we shouldn't ever call this when we are adding more, but just to be sure,
            // we'll avoid multiple enumeration
            var listInFile = initialListing.ToList();
            var smallerListings = listInFile.Where(s => s.Key.LineNumber < lineNumber).ToList();
            var largerListings = listInFile.Where(s => s.Key.LineNumber >= lineNumber).ToList();
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