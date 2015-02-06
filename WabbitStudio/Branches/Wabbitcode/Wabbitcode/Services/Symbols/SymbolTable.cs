using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using Revsoft.Wabbitcode.Properties;

namespace Revsoft.Wabbitcode.Services.Symbols
{
    public class SymbolTable
    {
        private ILookup<string, int> _labelToAddress;
        private ILookup<int, string> _addressToLabel;

        public IEnumerable<IGrouping<string, int>> Labels
        {
            get { return _labelToAddress; }
        }

        public List<string> GetLabelsFromAddress(int address)
        {
            return _addressToLabel == null ? new List<string>() : _addressToLabel[address].ToList();
        }

        public int? GetAddressFromLabel(string label)
        {
            if (!Settings.Default.CaseSensitive)
            {
                label = label.ToUpper();
            }

            if (_labelToAddress == null)
            {
                return null;
            }

            var labelVal = _labelToAddress[label].ToList();
            if (labelVal.Any())
            {
                return labelVal.Single();
            }

            return null;
        }

        public void ParseSymFile(string symFileContents)
        {
            var list = new List<KeyValuePair<string, int>>();

            var lines = symFileContents.Split('\n');
            foreach (string line in lines)
            {
                if (string.IsNullOrWhiteSpace(line))
                {
                    continue;
                }

                string[] labelAndAddress = line.Split('=');
                if (labelAndAddress.Length != 2)
                {
                    continue;
                }

                var address = labelAndAddress[0].Trim();
                var value = int.Parse(labelAndAddress[1].Trim().Substring(1), NumberStyles.HexNumber);
                list.Add(new KeyValuePair<string, int>(address, value));
            }

            _labelToAddress = list.ToLookup(kvp => kvp.Key, kvp => kvp.Value);
            _addressToLabel = list.ToLookup(kvp => kvp.Value & 0xFFFF, kvp => kvp.Key);
        }
    }
}