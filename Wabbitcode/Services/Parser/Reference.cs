namespace Revsoft.Wabbitcode.Services
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    public class Reference
    {
        private int col;
        private string file;
        private int line;
        private string lineContents;
        private string reference;

        public Reference(string file, int line, int col, string reference)
        {
            this.file = file;
            this.line = line;
            this.col = col;
            this.reference = reference;
        }

        public Reference(string file, int line, int col, string reference, string lineContents)
        {
            this.file = file;
            this.line = line;
            this.col = col;
            this.reference = reference;
            this.lineContents = lineContents;
        }

        public int Col
        {
            get
            {
                return this.col;
            }
            set
            {
                this.col = value;
            }
        }

        public string File
        {
            get
            {
                return this.file;
            }
            set
            {
                this.file = value;
            }
        }

        public int Line
        {
            get
            {
                return this.line;
            }
            set
            {
                this.line = value;
            }
        }

        public string LineContents
        {
            get
            {
                return this.lineContents;
            }
            set
            {
                this.lineContents = value;
            }
        }

        public string ReferenceString
        {
            get
            {
                return this.reference;
            }
        }
    }
}