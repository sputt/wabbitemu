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

		public IEnumerator<IParserData> GetEnumerator()
		{
			return (IEnumerator<IParserData>)new IParserEnumerator(this);
		}

		IEnumerator IEnumerable.GetEnumerator()
		{
			return (IEnumerator)new IParserEnumerator(this);
		}
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
				int count = 0;
				int newCount = parserData.LabelsList.Count - 1;
				if (position > newCount)
				{
					count = newCount;
					newCount += parserData.DefinesList.Count - 1;
					if (position > newCount)
					{
						count = newCount;
						newCount += parserData.MacrosList.Count - 1;
						if (position > newCount)
							return null;
						return parserData.MacrosList[position - count];
					}
					return parserData.DefinesList[position - count];
				}
				return parserData.LabelsList[position];
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
			int count = parserData.LabelsList.Count - 1;
			position++;
			if (position > count)
			{
				count += parserData.DefinesList.Count - 1;
				if (position > count)
				{
					count += parserData.MacrosList.Count - 1;
					if (position >= count)
						return false;
				}
			}
			return true;
		}

		public void Reset()
		{
			position = -1;
		}
	}
}
