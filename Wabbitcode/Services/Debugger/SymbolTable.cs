using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace Revsoft.Wabbitcode.Classes
{
    public class SymbolTable
    {
        private ILookup<string, string> labelToAddress;
        private ILookup<string, string> addressToLabel;

        public List<string> GetLabelsFromAddress(string address)
        {
            if (labelToAddress == null)
            {
                throw new Exception("No label file has been parsed");
            }
            return addressToLabel["$" + address].ToList();
        }

        public string GetAddressFromLabel(string label)
        {
            if (addressToLabel == null)
            {
                throw new Exception("No label file has been parsed");
            }
            return labelToAddress[label].SingleOrDefault();
        }

        public void ParseSymFile(string symFileContents)
        {
            List<KeyValuePair<string, string>> list = new List<KeyValuePair<string, string>>();

            string[] lines = symFileContents.Split('\n');
            foreach (string line in lines)
            {
                if (!string.IsNullOrWhiteSpace(line))
                {
                    string[] labelAndAddress = line.Split('=');
                    if (labelAndAddress.Length != 2)
                    {
                        continue;
                    }

                    list.Add(new KeyValuePair<string, string>(labelAndAddress[0].Trim(), labelAndAddress[1].Trim()));
                }
            }
            labelToAddress = list.ToLookup(kvp => kvp.Key, kvp => kvp.Value);
            addressToLabel = list.ToLookup(kvp => kvp.Value, kvp => kvp.Key);
        }
    }
}