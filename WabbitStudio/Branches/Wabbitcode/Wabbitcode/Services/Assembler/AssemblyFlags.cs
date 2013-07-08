namespace Revsoft.Wabbitcode.Services.Assembler
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    public enum AssemblyFlags
    {
        Normal = 1,
        CodeCounter = 2,
        Symtable = 4,
        Stats = 8,
        List = 16,
        Commandline = 32,
    }
}