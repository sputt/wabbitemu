using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services
{
    public class ParserFactory : IParserFactory
    {
        private static readonly Dictionary<string, Type> ParserDictionary = new Dictionary<string, Type>();

        public IParser CreateFromExtension(string extension)
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
        public void RegisterExtension(string extension, Type parserType)
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