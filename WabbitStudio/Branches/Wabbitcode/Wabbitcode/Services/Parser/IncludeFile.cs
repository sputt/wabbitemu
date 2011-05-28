using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Revsoft.Wabbitcode.Services.Parser
{
	class IncludeFile : IIncludeFile
	{
		public IncludeFile(DocLocation loc, string file, string description, ParserInformation parent)
		{
			Location = loc;
			IncludedFile = file;
			Description = description;
			Parent = parent;
		}

		public string Name
		{
			get { return IncludedFile; }
		}

		public DocLocation Location
		{
			get;
			set;
		}

		public string IncludedFile
		{
			get;
			set;
		}

		public string Description
		{
			get;
			set;
		}

		public ParserInformation Parent
		{
			get;
			set;
		}

        public override string ToString()
        {
            return Path.GetFileName(Name);
        }
	}
}
