using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Parser
{
	public class ParserInformation : IEnumerable<IParserData>
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

        ParserDataSorter dataSorter;

		public ParserInformation(string file)
		{
            dataSorter = new ParserDataSorter();
			sourceFile = file;
            GlobalDefinesList = new SortedSet<string>();
            IncludeFilesList = new SortedSet<IIncludeFile>(dataSorter as IComparer<IIncludeFile>);
            MacrosList = new SortedSet<IMacro>(dataSorter as IComparer<IMacro>);
            DefinesList = new SortedSet<IDefine>(dataSorter as IComparer<IDefine>);
            LabelsList = new SortedSet<ILabel>(dataSorter as IComparer<ILabel>);
		}

		/// <summary>
		/// All the defines created globally (outside of any file)
		/// </summary>
		public SortedSet<string> GlobalDefinesList
		{
			get;
			set;
		}

		/// <summary>
		/// Returns a list of all files included in the file.
		/// </summary>
        public SortedSet<IIncludeFile> IncludeFilesList
		{
			get;
			set;
		}

		public SortedSet<IMacro> MacrosList
		{
			get;
			set;
		}

        public SortedSet<IDefine> DefinesList
		{
			get;
			set;
		}

		/// <summary>
		/// List of all Labels in the file.
		/// </summary>
        public SortedSet<ILabel> LabelsList
		{
			get;
			set;
		}

        public IEnumerator<IParserData> GetEnumerator()
        {
            IParserData dataToReturn = null;
            IEnumerator includeEnumerator = IncludeFilesList.GetEnumerator();
            includeEnumerator.Reset();
            bool hasNextInclude = includeEnumerator.MoveNext();
            IEnumerator definesEnumerator = DefinesList.GetEnumerator();
            definesEnumerator.Reset();
            bool hasNextDefine = definesEnumerator.MoveNext();
            IEnumerator labelsEnumerator = LabelsList.GetEnumerator();
            labelsEnumerator.Reset();
            bool hasNextLabel = labelsEnumerator.MoveNext();
            IEnumerator macrosEnumerator = MacrosList.GetEnumerator();
            macrosEnumerator.Reset();
            bool hasNextMacro = macrosEnumerator.MoveNext();
            while (hasNextInclude || hasNextDefine || hasNextLabel || hasNextMacro)
            {
                if (hasNextInclude)
                {
                    dataToReturn = (IParserData) includeEnumerator.Current;
                }

            }
            return null;
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            throw new NotImplementedException();
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
            {
                return 0;
            }
            if (datax.Offset > datay.Offset)
            {
                return 1;
            }
            else
            {
                return -1;
            }
		}
	}
}
