using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
	public class ParserInformation //: IEnumerable<IParserData>
	{
		private string sourceFile;
		public string SourceFile
		{
			get { return sourceFile; }
		}

        private bool isIncluded = false;
        public bool IsIncluded
        {
            get { return isIncluded; }
            set { isIncluded = value; }
        }
		public ParserInformation(string file)
		{
			sourceFile = file;
			//IncludeDirList = new List<string>();
			GlobalDefinesList = new List<string>();
			IncludeFilesList = new List<IIncludeFile>();
			MacrosList = new List<IMacro>();
			DefinesList = new List<IDefine>();
			LabelsList = new List<ILabel>();
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

		public List<IMacro> MacrosList
		{
			get;
			set;
		}

		public List<IDefine> DefinesList
		{
			get;
			set;
		}

		/// <summary>
		/// List of all Labels in the file.
		/// </summary>
		public List<ILabel> LabelsList
		{
			get;
			set;
		}

        IParserData[] generatedList;
        public IParserData[] GeneratedList
        {
            get
            {
                int counter = 0;
                int size = LabelsList.Count + DefinesList.Count +
                            IncludeFilesList.Count + MacrosList.Count;
                generatedList = new IParserData[size];
                foreach (IParserData label in LabelsList)
                {
                    generatedList[counter] = label;
                    counter++;
                }
                foreach (IParserData define in DefinesList)
                {
                    generatedList[counter] = define;
                    counter++;
                }
                foreach (IParserData include in IncludeFilesList)
                {
                    generatedList[counter] = include;
                    counter++;
                }
                foreach (IParserData macro in MacrosList)
                {
                    generatedList[counter] = macro;
                    counter++;
                }
                ParserDataSorter sorter = new ParserDataSorter();
                Array.Sort(generatedList, sorter);
                return generatedList;
            }
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

	public class IParserEnumerator : IEnumerator<IParserData>
	{
		private ParserInformation parserData;
		private int position = -1;
		public IParserEnumerator(ParserInformation data)
		{
			parserData = data;            
		}

		public IParserData Current
		{
			get
			{
                return parserData.GeneratedList[position];
			}
		}

		public void Dispose()
		{
			return;
		}

		object IEnumerator.Current
		{
			get { return position; }
		}

		public bool MoveNext()
		{
			position++;
            return position < parserData.GeneratedList.Length;
		}

		public void Reset()
		{
			position = -1;
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

            if (datax == null || datay == null || datax.Offset == datay.Offset)
                return 0;
            if (datax.Offset > datay.Offset)
                return 1;
            else
                return -1;
        }
    }
}
