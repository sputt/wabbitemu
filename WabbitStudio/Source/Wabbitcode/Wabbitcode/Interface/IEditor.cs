using System;
using Revsoft.Wabbitcode.Utilities;

namespace Revsoft.Wabbitcode.Interface
{
	interface IEditor
	{
		FilePath FilePath { get; }

		void OpenFile(string filename);
		void OpenFile(FilePath filename);
		void SaveFile();

		void SelectAll();
	}
}
