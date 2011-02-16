using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Threading;
using System.Threading;
using System.Xml;

namespace Revsoft.Wabbitcode
{
    public static class ExtensionMethods
    {
        public static bool InvokeRequired(this Dispatcher dispatcher)
        {
            return dispatcher.Thread != Thread.CurrentThread;
        }

        public static bool MoveToNextElement(this XmlTextReader reader)
        {
            if (!reader.Read())
                return false;

            while (reader.NodeType == XmlNodeType.EndElement)
            {
                if (!reader.Read())
                    return false;
            }

            return true;
        }
    }
}
