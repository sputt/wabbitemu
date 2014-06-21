using System;
using System.Diagnostics.CodeAnalysis;

namespace Revsoft.Wabbitcode.Services.Assembler
{
    [SuppressMessage("Microsoft.Naming", "CA1726:UsePreferredTerms", MessageId = "Flags")]
    [Flags]
    public enum AssemblyFlags
    {
        Normal = 1,
        CodeCounter = 2,
        SymbolTable = 4,
        Stats = 8,
        List = 16,
        Commandline = 32,
    }
}