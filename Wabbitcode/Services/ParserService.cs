using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public class ParserService : IParserService
    {
        private readonly Dictionary<string, ParserInformation> _parserInfoDictionary = new Dictionary<string, ParserInformation>();
        private readonly IFileService _fileService;
        private readonly IParserFactory _parserFactory;

        #region Events

        public event ParserProgressHandler OnParserProgress;
        public event ParserFinishedHandler OnParserFinished;

        #endregion

        public ParserService(IFileService fileService, IParserFactory parserFactory)
        {
            _fileService = fileService;
            _parserFactory = parserFactory;

            _parserFactory.RegisterExtension(".asm", typeof(Z80Parser));
            _parserFactory.RegisterExtension(".z80", typeof(Z80Parser));
            _parserFactory.RegisterExtension(".inc", typeof(Z80Parser));
        }

        /// <summary>
        /// Finds all references to the given text.
        /// </summary>
        /// <param name="file">Fully rooted path to the file</param>
        /// <param name="refString">String to find references to</param>
        public List<Reference> FindAllReferencesInFile(FilePath file, string refString)
        {
            if (string.IsNullOrEmpty(file))
            {
                Logger.Log("No file name specified");
                return new List<Reference>();
            }

            string extension = Path.GetExtension(file);
            IParser parser = _parserFactory.CreateFromExtension(extension);
            if (parser == null)
            {
                return new List<Reference>();
            }

            string fileText = _fileService.GetFileText(file);
            return parser.FindReferences(file, fileText, refString, Settings.Default.CaseSensitive);
        }

        public IEnumerable<IParserData> GetAllParserData()
        {
            return _parserInfoDictionary.Values.SelectMany(info => info);
        }

        public IEnumerable<IParserData> GetParserData(string referenceString, bool caseSensitive)
        {
            if (string.IsNullOrEmpty(referenceString))
            {
                yield break;
            }

            var parserData = _parserInfoDictionary.Values.ToList();
            StringComparison comparison = caseSensitive ? StringComparison.CurrentCulture : StringComparison.CurrentCultureIgnoreCase;
            foreach (var data in parserData.SelectMany(
                info => info.Where(data => data.Name.Equals(referenceString, comparison)))
                )
            {
                yield return data;
            }
        }

        public void ParseFile(int hashCode, FilePath filename)
        {
            string fileText = _fileService.GetFileText(filename);
            ParseFile(hashCode, filename, fileText);
        }

        public void ParseFile(int hashCode, FilePath file, string fileText)
        {
            if (file == null)
            {
                System.Diagnostics.Debug.WriteLine("No file name specified");
                return;
            }

            if (string.IsNullOrEmpty(fileText))
            {
                System.Diagnostics.Debug.WriteLine("Lines were null or empty");
                return;
            }

            if (hashCode == 0)
            {
                hashCode = fileText.GetHashCode();
            }

            string extension = Path.GetExtension(file);
            IParser parser = _parserFactory.CreateFromExtension(extension);
            if (parser == null)
            {
                return;
            }

            ParserInformation parserInfo = new ParserInformation(hashCode, file);
            parser.ParseText(parserInfo, fileText);

            lock (_parserInfoDictionary)
            {
                _parserInfoDictionary.Remove(file);
                _parserInfoDictionary.Add(file, parserInfo);
                foreach (var item in _parserInfoDictionary)
                {
                    item.Value.IsIncluded = false;
                }
            }

            if (OnParserFinished != null)
            {
                OnParserFinished(this, new ParserEventArgs(file));
            }
        }

        public ParserInformation GetParserInfo(FilePath fileName)
        {
            lock (_parserInfoDictionary)
            {
                ParserInformation info;
                _parserInfoDictionary.TryGetValue(fileName, out info);
                return info;
            }
        }

        public IParserData TryResolveDefine(IDefine parserData, bool caseSensitive)
        {
            string contents = parserData.Contents;
            if (contents.EndsWith("("))
            {
                contents = contents.Remove(contents.Length - 1);
            }

            var possibleData = GetParserData(contents, caseSensitive).ToList();
            var resolvedType = possibleData.FirstOrDefault();
            return resolvedType;
        }
    }
}