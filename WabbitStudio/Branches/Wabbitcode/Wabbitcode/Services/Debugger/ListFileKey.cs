using System;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public class ListFileKey : IEquatable<ListFileKey>
    {
        public ListFileKey(string file, int lineNumber)
        {
            this.FileName = file;
            this.LineNumber = lineNumber;
        }

        public string FileName
        {
            get;
            private set;
        }

        public int LineNumber
        {
            get;
            private set;
        }

        public override bool Equals(object obj)
        {
            if (!(obj is ListFileKey))
            {
                return false;
            }
            ListFileKey key = obj as ListFileKey;
            return key.FileName == this.FileName && key.LineNumber == this.LineNumber;
        }

        public bool Equals(ListFileKey key)
        {
            return key.FileName == this.FileName && key.LineNumber == this.LineNumber;
        }

        public override int GetHashCode()
        {
            return this.FileName.Length + this.LineNumber;
        }

        public override string ToString()
        {
            return "File: " + this.FileName + " Line: " + this.LineNumber;
        }
    }
}