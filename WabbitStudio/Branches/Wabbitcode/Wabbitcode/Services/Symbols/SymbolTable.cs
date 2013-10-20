using System.Collections.Generic;
using System.Linq;
using Revsoft.Wabbitcode.Properties;

namespace Revsoft.Wabbitcode.Services.Symbols
{
	public class SymbolTable
	{
		private ILookup<string, string> _labelToAddress;
		private ILookup<string, string> _addressToLabel;

		public List<string> GetLabelsFromAddress(string address)
		{
			return _addressToLabel == null ? new List<string>() : _addressToLabel[address].ToList();
		}

		public string GetAddressFromLabel(string label)
		{
		    if (!Settings.Default.caseSensitive)
		    {
		        label = label.ToUpper();
		    }

			return _addressToLabel == null ? string.Empty : _labelToAddress[label].SingleOrDefault();
		}

		public void ParseSymFile(string symFileContents)
		{
			List<KeyValuePair<string, string>> list = new List<KeyValuePair<string, string>>();

			string[] lines = symFileContents.Split('\n');
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

				list.Add(new KeyValuePair<string, string>(labelAndAddress[0].Trim(), labelAndAddress[1].Trim().Substring(1)));
			}
			_labelToAddress = list.ToLookup(kvp => kvp.Key, kvp => kvp.Value);
			_addressToLabel = list.ToLookup(kvp => kvp.Value, kvp => kvp.Key);
		}
	}
}