using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using Revsoft.TextEditor.Document;

namespace Revsoft.TextEditor.Src.Document.HighlightingStrategy.SyntaxModes
{
    public class StreamSyntaxModeProvider : ISyntaxModeFileProvider
    {
        private readonly Dictionary<SyntaxMode, Stream> _syntaxModes = new Dictionary<SyntaxMode, Stream>();
		
		public ICollection<SyntaxMode> SyntaxModes {
			get {
				return _syntaxModes.Keys;
			}
		}

        public void AddSyntaxMode(SyntaxMode syntaxMode, Stream stream)
        {
            if (syntaxMode == null)
            {
                throw new NullReferenceException("syntaxMode");
            }

            if (stream == null)
            {
                throw new NullReferenceException("stream");
            }

            _syntaxModes.Add(syntaxMode, stream);
        }

        public XmlTextReader GetSyntaxModeFile(SyntaxMode syntaxMode)
        {
            Stream stream;
            _syntaxModes.TryGetValue(syntaxMode, out stream);
            if (stream == null)
            {
                throw new HighlightingDefinitionInvalidException("Can't load highlighting definition " + syntaxMode.Name);
            }

            return new XmlTextReader(stream);
        }
		
		public void UpdateSyntaxModeList()
		{
			// resources don't change during runtime
		}
    }
}
