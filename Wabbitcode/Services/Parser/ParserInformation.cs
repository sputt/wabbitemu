namespace Revsoft.Wabbitcode.Services.Parser
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    public class IParserEnumerator : IEnumerator<IParserData>, IDisposable
    {
        private ParserInformation parserData;
        private int position = -1;

        public IParserEnumerator(ParserInformation data)
        {
            this.parserData = data;
        }

        public IParserData Current
        {
            get
            {
                return this.parserData.GeneratedList[this.position];
            }
        }

        object IEnumerator.Current
        {
            get
            {
                return this.position;
            }
        }

        public virtual void Dispose()
        {
            return;
        }

        public bool MoveNext()
        {
            this.position++;
            return this.position < this.parserData.GeneratedList.Length;
        }

        public void Reset()
        {
            this.position = -1;
        }
    }

    public class ParserDataSorter : IComparer
    {
        // Compare the length of the strings, or the strings
        // themselves, if they are the same length.
        public int Compare(object x, object y)
        {
            IParserData datax = x as IParserData;
            IParserData datay = y as IParserData;

            if (datax == null || datay == null || datax.Location.Offset == datay.Location.Offset)
            {
                return 0;
            }
            if (datax.Location.Offset > datay.Location.Offset)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }
    }

    // : IEnumerable<IParserData>
    public class ParserInformation
    {
        private IParserData[] generatedList;
        private bool isIncluded = false;
        private bool parsingIncludes = false;
        private string sourceFile;
        private int sourceFileHash;

        public ParserInformation(int fileHashCode)
        : this(fileHashCode, null)
        {
        }

        public ParserInformation(int fileHashCode, string file)
        {
            this.sourceFileHash = fileHashCode;
            this.sourceFile = file;

            // IncludeDirList = new List<string>();
            this.GlobalDefinesList = new List<string>();
            this.IncludeFilesList = new List<IIncludeFile>();
            this.MacrosList = new List<IMacro>();
            this.DefinesList = new List<IDefine>();
            this.LabelsList = new List<ILabel>();
        }

        public List<IDefine> DefinesList
        {
            get;
            set;
        }

        public IParserData[] GeneratedList
        {
            get
            {
                int counter = 0;
                int size = this.LabelsList.Count + this.DefinesList.Count +
                           this.IncludeFilesList.Count + this.MacrosList.Count;
                this.generatedList = new IParserData[size];
                foreach (IParserData label in this.LabelsList)
                {
                    this.generatedList[counter] = label;
                    counter++;
                }

                foreach (IParserData define in this.DefinesList)
                {
                    this.generatedList[counter] = define;
                    counter++;
                }

                foreach (IParserData include in this.IncludeFilesList)
                {
                    this.generatedList[counter] = include;
                    counter++;
                }

                foreach (IParserData macro in this.MacrosList)
                {
                    this.generatedList[counter] = macro;
                    counter++;
                }

                ParserDataSorter sorter = new ParserDataSorter();
                Array.Sort(this.generatedList, sorter);
                return this.generatedList;
            }
        }

        /// <summary>
        /// This is all the directories to include with spasm
        /// </summary>
        /*public List<string> IncludeDirList
        {
            get;
            set;
        }*/
        /// <summary>
        /// All the defines created globally (outside of any file)
        /// </summary>
        public List<string> GlobalDefinesList
        {
            get;
            set;
        }

        /// <summary>
        /// Returns a list of all files included in the file.
        /// </summary>
        public List<IIncludeFile> IncludeFilesList
        {
            get;
            set;
        }

        public bool IsIncluded
        {
            get
            {
                return this.isIncluded;
            }
            set
            {
                this.isIncluded = value;
            }
        }

        /// <summary>
        /// List of all Labels in the file.
        /// </summary>
        public List<ILabel> LabelsList
        {
            get;
            set;
        }

        public List<IMacro> MacrosList
        {
            get;
            set;
        }

        public bool ParsingIncludes
        {
            get
            {
                return this.parsingIncludes;
            }
            set
            {
                this.parsingIncludes = value;
            }
        }

        public string SourceFile
        {
            get
            {
                return this.sourceFile;
            }
        }

        public int SourceFileHash
        {
            get
            {
                return this.sourceFileHash;
            }
        }

        public override string ToString()
        {
            return this.sourceFile;
        }

        /*public IEnumerator<IParserData> GetEnumerator()
        {
            return (IEnumerator<IParserData>)new IParserEnumerator(this);
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return (IEnumerator)new IParserEnumerator(this);
        }*/
    }
}