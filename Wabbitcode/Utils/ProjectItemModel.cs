using System;

namespace Revsoft.Wabbitcode.Utils
{
    public class ProjectItemModel
    {
        public string Ext { get; private set; }

        public string File { get; private set; }

        public string Text { get; private set; }

        public bool UsesIcon { get; private set; }

        public ProjectItemModel(string file, string ext, string text, bool usesIcon)
        {
            File = file;
            Ext = ext;
            Text = text;
            UsesIcon = usesIcon;
        }

        public override string ToString()
        {
            return Text;
        }
    }
}