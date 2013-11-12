using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Revsoft.Wabbitcode.Services
{
    [ServiceDependency(typeof(IFileReaderService))]
    public class ParserService : IParserService
    {
        #region Events

        public event ParserProgressHandler OnParserProgress;
        public event ParserFinishedHandler OnParserFinished;

        #endregion

        private readonly Dictionary<string, ParserInformation> _parserInfoDictionary = new Dictionary<string, ParserInformation>();
        private readonly IFileReaderService _fileReaderService;

        /// <summary>
        /// Finds all references to the given text.
        /// </summary>
        /// <param name="file">Fully rooted path to the file</param>
        /// <param name="refString">String to find references to</param>
        public List<Reference> FindAllReferencesInFile(string file, string refString)
        {
            if (string.IsNullOrEmpty(file))
            {
                System.Diagnostics.Debug.WriteLine("No file name specified");
                return new List<Reference>();
            }

            string extension = Path.GetExtension(file);
            IParser parser = ParserFactory.CreateFromExtension(extension);
            if (parser == null)
            {
                return new List<Reference>();
            }

            string fileText = _fileReaderService.GetFileText(file);
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

            StringComparison comparison = caseSensitive ? StringComparison.CurrentCulture : StringComparison.CurrentCultureIgnoreCase;
            foreach (var data in _parserInfoDictionary.Values.SelectMany(
                info => info.Where(data => data.Name.Equals(referenceString, comparison)))
                )
            {
                yield return data;
            }
        }

        public void ParseFile(int hashCode, string filename)
        {
            string fileText = _fileReaderService.GetFileText(filename);
            ParseFile(hashCode, filename, fileText);
        }

        public void ParseFile(int hashCode, string file, string fileText)
        {
            if (string.IsNullOrEmpty(file))
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
            IParser parser = ParserFactory.CreateFromExtension(extension);
            if (parser == null)
            {
                return;
            }

            ParserInformation parserInfo = new ParserInformation(hashCode, file);
            parser.ParseText(parserInfo, fileText);

            lock (_parserInfoDictionary)
            {
                _parserInfoDictionary.Remove(file.ToLower());
                _parserInfoDictionary.Add(file.ToLower(), parserInfo);
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

        public ParserInformation GetParserInfo(string fileName)
        {
            lock (_parserInfoDictionary)
            {
                ParserInformation info;
                _parserInfoDictionary.TryGetValue(fileName.ToLower(), out info);
                return info;
            }
        }

        public ParserService(IFileReaderService fileReaderService)
        {
            _fileReaderService = fileReaderService;
        }

        public void DestroyService()
        {

        }

        public void InitService(params object[] objects)
        {
            ParserFactory.RegisterExtension(".asm", typeof(Z80Parser));
            ParserFactory.RegisterExtension(".z80", typeof(Z80Parser));
            ParserFactory.RegisterExtension(".inc", typeof(Z80Parser));
        }
    }

    public static class ParserFactory
    {
        private static readonly Dictionary<string, Type> ParserDictionary = new Dictionary<string, Type>();

        internal static IParser CreateFromExtension(string extension)
        {
            Type parserType;
            return !ParserDictionary.TryGetValue(extension, out parserType) ? null : (IParser) Activator.CreateInstance(parserType);
        }

        /// <summary>
        /// Registers a new parser based on the specified extension.
        /// </summary>
        /// <param name="extension">The extension to create the parser for</param>
        /// <param name="parserType">The parser </param>
        /// <exception cref="ArgumentException">Thrown if the extension is already registered.</exception>
        /// <exception cref="ArgumentException">Thrown if the parser is not of type IParser</exception>
        /// <exception cref="NullReferenceException">Thrown if the parser or extension is null</exception>
        public static void RegisterExtension(string extension, Type parserType)
        {
            if (string.IsNullOrWhiteSpace(extension))
            {
                throw new NullReferenceException("Extension is null or empty");
            }

            if (parserType == null)
            {
                throw new NullReferenceException("Parser type is null");
            }

            if (parserType.IsInstanceOfType(typeof(IParser)))
            {
                throw new ArgumentException("Parser is not of type IParser");
            }

            if (ParserDictionary.ContainsKey(extension))
            {
                throw new ArgumentException("Extension is already registered");
            }

            ParserDictionary.Add(extension, parserType);
        }
    }
}