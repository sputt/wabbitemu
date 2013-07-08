namespace Revsoft.Wabbitcode.Services.Parser
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Text;

    internal class IncludeFile : IIncludeFile
    {
        public IncludeFile(DocLocation loc, string file, string description, ParserInformation parent)
        {
            this.Location = loc;
            this.IncludedFile = file;
            this.Description = description;
            this.Parent = parent;
        }

        public string Description
        {
            get;
            set;
        }

        public string IncludedFile
        {
            get;
            set;
        }

        public DocLocation Location
        {
            get;
            set;
        }

        public string Name
        {
            get
            {
                return this.IncludedFile;
            }
        }

        public ParserInformation Parent
        {
            get;
            set;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is IIncludeFile))
            {
                return base.Equals(obj);
            }
            var include = (IIncludeFile)obj;
            return string.Equals(include.IncludedFile, this.IncludedFile, StringComparison.OrdinalIgnoreCase);
        }

        public override int GetHashCode()
        {
            return this.Name.GetHashCode();
        }

        public override string ToString()
        {
            return Path.GetFileName(this.Name);
        }
    }
}