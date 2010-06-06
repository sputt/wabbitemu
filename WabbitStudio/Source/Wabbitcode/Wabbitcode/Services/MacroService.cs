using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Classes
{
	public static class MacroService
	{
		public static List<string> macros = new List<string>();
		public static int currentMacro = -1;

		public static bool IsRecording { get; set; }

		internal static void RecordKeyData(System.Windows.Forms.Keys keyData)
		{
			throw new NotImplementedException();
		}
	}
}
