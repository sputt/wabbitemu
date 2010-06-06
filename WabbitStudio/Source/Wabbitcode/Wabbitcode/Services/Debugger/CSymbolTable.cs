using System.Collections;
using System.Collections.Generic;

namespace Revsoft.Wabbitcode.Classes
{
    public class SymbolTableClass
    {
        public Hashtable staticLabels;
        public void ParseSymFile(string symFileContents)
        {
            staticLabels = new Hashtable();
            string[] lines = symFileContents.Split('\n');
            foreach (string line in lines)
            {
                if (!string.IsNullOrEmpty(line))
                {
                    int equalsLoc = line.IndexOf('=');
                    staticLabels.Add(line.Substring(0, equalsLoc - 1),
                                     line.Substring(equalsLoc + 2, line.Length - equalsLoc - 3));
                }
            }
        }

        public List<string> FindEntryByValue(string value)
        {
            List<string> possibleEntries = new List<string>();
            foreach (DictionaryEntry entry in staticLabels)
                if (entry.Value.ToString() == "$" + value)
                    possibleEntries.Add(entry.Key.ToString());
            return possibleEntries;
        }
    }
}
