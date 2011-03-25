using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Diagnostics;

namespace WabbitC
{
    class Program
    {
        enum CompilerStatus
        {
            Normal,
            Warnings,
            Error
        }
        static string InputFile;
        static string OutputFile;
        static CompilerStatus Status;
		
		[STAThread]
        static void Main(string[] args)
        {
			Compiler.OptimizeLevel opLevel = Compiler.OptimizeLevel.OptimizeNone;
            foreach (string arg in args)
            {
                if (string.IsNullOrEmpty(arg))
                    continue;
                if (arg[0] == '-')
                {
                    switch (arg[1])
                    {
                        case 'O':
							if (!char.IsDigit(arg[2]))
								throw new InvalidDataException("-O switch doesn not have an appropriate optimization level");
							int optimizeLevel = int.Parse(arg[2].ToString());
							switch (optimizeLevel)
							{
								case 1:
									opLevel = Compiler.OptimizeLevel.OptimizeSome;
									break;
								case 2:
									opLevel = Compiler.OptimizeLevel.OptimizeAlot;
									break;
								case 3:
									opLevel = Compiler.OptimizeLevel.OptimizeMax;
									break;
							}
                            break;
                    }
                }
                else
                {
                    if (string.IsNullOrEmpty(InputFile))
                        InputFile = arg;
                    else
                        OutputFile = arg;
                }
            }
			IncludeDirs.LocalIncludes.Insert(0, Environment.CurrentDirectory);
            Compiler.DoCompile(InputFile, opLevel);
        }
    }
}
