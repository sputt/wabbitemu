using System;
using System.Collections.Generic;

namespace Revsoft.Wabbitcode.Services
{
    public static class MacroService
    {
        public static int currentMacro = -1;
        public static List<string> macros = new List<string>();

        public static bool IsRecording
        {
            get;
            set;
        }

        internal static void RecordKeyData(System.Windows.Forms.Keys keyData)
        {
            throw new NotImplementedException();
        }
    }
}