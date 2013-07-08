namespace Revsoft.Wabbitcode
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    public class ListBoxItem
    {
        public string Ext
        {
            get;
            set;
        }

        public string File
        {
            get;
            set;
        }

        public string Text
        {
            get;
            set;
        }

        public override string ToString()
        {
            return this.Text;
        }
    }
}