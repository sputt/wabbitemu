using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ICSharpCode.AvalonEdit;

namespace Revsoft.Wabbitcode.AvalonEditExtension.Src.Tasks
{
    public sealed class TagComment
    {
        string key;
        public string Key
        {
            get { return key; }
        }

        string commentString;
        public string CommentString
        {
            get { return commentString; }
        }

        TextViewPosition pos;
        public TextViewPosition Pos
        {
            get { return pos; }
        }

        public TagComment(string key, TextViewPosition pos)
        {
            this.key = key;
            this.pos = pos;
        }

        public TagComment(string key, TextViewPosition pos, string commentString)
        {
            this.key = key;
            this.pos = pos;
            this.commentString = commentString;
        }
    }
}
