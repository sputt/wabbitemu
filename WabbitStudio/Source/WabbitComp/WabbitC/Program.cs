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
        static void Main(string[] args)
        {
            foreach (string arg in args)
            {
                if (string.IsNullOrEmpty(arg))
                    continue;
                if (arg[0] == '-')
                {
                    switch (arg[1])
                    {
                        case 'O':

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
            Compiler.DoCompile(InputFile);
        }
    }
}
