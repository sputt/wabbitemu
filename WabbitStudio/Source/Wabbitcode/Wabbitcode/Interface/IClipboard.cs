using System;

namespace Revsoft.Wabbitcode.Interface
{
	public interface IClipboard
	{
		void Copy();
		void Cut();
		void Paste();
	}
}
