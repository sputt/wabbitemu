using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.AvalonEditExtension
{
    public class ExtensionMethods
    {
        /// <summary>
        /// Returns the index of the first element for which <paramref name="predicate"/> returns true.
        /// If none of the items in the list fits the <paramref name="predicate"/>, -1 is returned.
        /// </summary>
        public static int FindIndex<T>(this IList<T> list, Func<T, bool> predicate)
        {
            for (int i = 0; i < list.Count; i++)
            {
                if (predicate(list[i]))
                    return i;
            }

            return -1;
        }
    }
}
