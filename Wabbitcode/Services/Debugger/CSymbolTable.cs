namespace Revsoft.Wabbitcode.Classes
{
    using System.Collections;
    using System.Collections.Generic;

    public class SymbolTableClass
    {
        private Hashtable staticLabels;

        public Hashtable StaticLabels
        {
            get
            {
                return this.staticLabels;
            }
        }

        public List<string> FindEntryByValue(string value)
        {
            List<string> possibleEntries = new List<string>();
            foreach (DictionaryEntry entry in this.staticLabels)
            {
                if (entry.Value.ToString() == "$" + value)
                {
                    possibleEntries.Add(entry.Key.ToString());
                }
            }
            return possibleEntries;
        }

        public void ParseSymFile(string symFileContents)
        {
            this.staticLabels = new Hashtable();
            string[] lines = symFileContents.Split('\n');
            foreach (string line in lines)
            {
                if (!string.IsNullOrEmpty(line))
                {
                    int equalsLoc = line.IndexOf('=');
                    this.staticLabels.Add(
                        line.Substring(0, equalsLoc - 1),
                        line.Substring(equalsLoc + 2, line.Length - equalsLoc - 3));
                }
            }
        }
    }
}