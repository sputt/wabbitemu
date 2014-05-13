using System;

namespace Revsoft.Wabbitcode.Services.Assembler
{
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