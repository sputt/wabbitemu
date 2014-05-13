namespace Revsoft.Wabbitcode.Services.Parser
{
    using System;
    using System.IO;

    internal class IncludeFile : IIncludeFile
    {
        public IncludeFile(DocLocation loc, string file, string description, ParserInformation parent)
        {
            Location = loc;
            IncludedFile = file;
            Description = description;
            Parent = parent;
        }

        public string Description { get; set; }

        public string IncludedFile { get; set; }

        public DocLocation Location { get; set; }

        public string Name
        {
            get { return IncludedFile; }
        }

        public ParserInformation Parent { get; set; }

        public override bool Equals(object obj)
        {
            if (!(obj is IIncludeFile))
            {
                return base.Equals(obj);
            }
            var include = (IIncludeFile) obj;
            return string.Equals(include.IncludedFile, IncludedFile, StringComparison.OrdinalIgnoreCase);
        }

        public override int GetHashCode()
        {
            return Name.GetHashCode();
        }

        public override string ToString()
        {
            return Name == null ? Name : Path.GetFileName(Name);
        }
    }
}