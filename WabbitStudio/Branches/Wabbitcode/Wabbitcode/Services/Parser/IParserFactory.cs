using System;

namespace Revsoft.Wabbitcode.Services.Parser
{
    public interface IParserFactory
    {
        IParser CreateFromExtension(string extension);

        /// <summary>
        /// Registers a new parser based on the specified extension.
        /// </summary>
        /// <param name="extension">The extension to create the parser for</param>
        /// <param name="parserType">The parser </param>
        /// <exception cref="ArgumentException">Thrown if the extension is already registered.</exception>
        /// <exception cref="ArgumentException">Thrown if the parser is not of type IParser</exception>
        /// <exception cref="NullReferenceException">Thrown if the parser or extension is null</exception>
        void RegisterExtension(string extension, Type parserType);
    }
}