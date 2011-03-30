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
            Compiler.OptimizeLevel opLevel = Optimizer.Optimizer.ParseCommandLine(args);
            foreach (string arg in args)
            {
                if (string.IsNullOrEmpty(arg))
                {
                    continue;
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
