using System.Collections.Generic;
using System.Linq;

namespace Revsoft.Wabbitcode.Services.Symbols
{
	public class SymbolTable
	{
		private ILookup<string, string> _labelToAddress;
		private ILookup<string, string> _addressToLabel;

		public List<string> GetLabelsFromAddress(string address)
		{
			return _labelToAddress == null ? new List<string>() : _addressToLabel["$" + address].ToList();
		}

		public string GetAddressFromLabel(string label)
		{
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

				list.Add(new KeyValuePair<string, string>(labelAndAddress[0].Trim(), labelAndAddress[1].Trim()));
			}
			_labelToAddress = list.ToLookup(kvp => kvp.Key, kvp => kvp.Value);
			_addressToLabel = list.ToLookup(kvp => kvp.Value, kvp => kvp.Key);
		}
	}
}