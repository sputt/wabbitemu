using Revsoft.Wabbitcode.Services.Debugger;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using System.Xml;


namespace Revsoft.Wabbitcode.Classes
{
    public static class ExtensionMethods
    {
        public static bool Contains(this string source, string toCheck, StringComparison comp)
        {
            return source.IndexOf(toCheck, comp) >= 0;
        }

        public static void Invoke(this Control control, Action action)
        {
            control.Invoke((Delegate)action);
        }

        public static bool MoveToNextElement(this XmlTextReader reader)
        {
            if (!reader.Read())
            {
                return false;
            }

            while (reader.NodeType == XmlNodeType.EndElement)
            {
                if (!reader.Read())
                {
                    return false;
                }
            }

            return true;
        }

        public static Bitmap ResizeImage(this Bitmap img, int width, int height)
        {
            Bitmap resized = new Bitmap(width + 1, height + 1);
            Graphics g = Graphics.FromImage(resized);
            g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;
            g.DrawImage(img, 0, 0, width + 1, height + 1);
            g.Dispose();

            return resized;
        }

        /// <summary>
        /// Convert a Color to a hex string.
        /// </summary>
        /// <returns>ex: "#FFFFFF", "#AB12E9"</returns>
        public static string ToHexString(this Color color)
        {
            return "#" + color.R.ToString("X2") + color.G.ToString("X2") + color.B.ToString("X2");
        }

        public static bool TryGetKey(this IDictionary<ListFileKey, ListFileValue> lookup, ListFileValue value, out ListFileKey key)
        {
            key = null;
            foreach (var pair in lookup)
            {
                if (pair.Value.Equals(value))
                {
                    key = pair.Key;
                }
            }

            return key != null;
        }
    }
}