using System.Collections;
using System.Collections.Generic;
using System.Linq;


namespace Revsoft.Wabbitcode.Services.Parser
{
    public class ParserInformation : IEnumerable<IParserData>
    {
	    private bool _parsingIncludes;
        private readonly string _sourceFile;
        private readonly int _sourceFileHash;

        public ParserInformation(int fileHashCode, string file)
        {
            _sourceFileHash = fileHashCode;
            _sourceFile = file;

            IncludeFilesList = new List<IIncludeFile>();
            MacrosList = new List<IMacro>();
            DefinesList = new List<IDefine>();
            LabelsList = new List<ILabel>();
        }

        public List<IDefine> DefinesList
        {
            get; private set;
        }

        /// <summary>
        /// Returns a list of all files included in the file.
        /// </summary>
        public List<IIncludeFile> IncludeFilesList
        {
            get; private set;
        }

	    public bool IsIncluded { get; set; }

	    /// <summary>
        /// List of all Labels in the file.
        /// </summary>
        public List<ILabel> LabelsList
        {
            get; private set;
        }

        public List<IMacro> MacrosList
        {
            get; private set;
        }

        public string SourceFile
        {
            get
            {
                return _sourceFile;
            }
        }

        public override string ToString()
        {
            return _sourceFile;
        }

        public IEnumerator<IParserData> GetEnumerator()
        {
	        foreach (IDefine define in DefinesList)
	        {
		        yield return define;
	        }

	        foreach (ILabel label in LabelsList)
	        {
		        yield return label;
	        }

	        foreach (IMacro macro in MacrosList)
	        {
		        yield return macro;
	        }

	        foreach (IIncludeFile includeFile in IncludeFilesList)
	        {
		        yield return includeFile;
	        }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
	        return GetEnumerator();
        }
    }
}