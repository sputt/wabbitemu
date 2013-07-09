namespace Revsoft.Wabbitcode.Services.Assembler
{
    using Revsoft.Wabbitcode.Classes;
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;

    public class SpasmExeAssembler : IAssembler
    {
        private const string quote = "\"";

        private bool caseSensitive;
        private List<KeyValuePair<string, string>> defines = new List<KeyValuePair<string, string>>();
        private string flagsString = String.Empty;
        private List<string> includeDirs = new List<string>();
        private string inputFile;
        private string outputFile;
        private Process wabbitspasm;

        public SpasmExeAssembler()
        {
            // create two new processes to run
            // setup wabbitspasm to run silently
            this.wabbitspasm = new Process
            {
                StartInfo =
                {
                    FileName = FileLocations.SpasmFile,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true
                }
            };
        }

        public void AddDefine(string name, string value)
        {
            this.defines.Add(new KeyValuePair<string, string>(name, value));
        }

        public void AddIncludeDir(string path)
        {
            if (string.IsNullOrEmpty(path))
            {
                return;
            }

            this.includeDirs.Add(path);
        }

        public string Assemble()
        {
            string includeDir = "-I ";
            foreach (string dir in this.includeDirs)
            {
                includeDir += quote + dir + quote + ";";
            }

            string caseString = this.caseSensitive ? " -A " : " ";
            this.wabbitspasm.StartInfo.Arguments = includeDir + caseString + this.flagsString + quote + this.inputFile + quote + " " + quote + this.outputFile + quote;
            this.wabbitspasm.Start();
            return this.wabbitspasm.StandardOutput.ReadToEnd();
        }

        public void ClearDefines()
        {
            this.defines.Clear();
        }

        public void ClearIncludeDirs()
        {
            this.includeDirs.Clear();
        }

        public void Dispose()
        {
            this.wabbitspasm.Dispose();
        }

        public void SetCaseSensitive(bool caseSensitive)
        {
            this.caseSensitive = caseSensitive;
        }

        public void SetFlags(AssemblyFlags flags)
        {
            if ((flags & AssemblyFlags.CodeCounter) != 0)
            {
                this.flagsString += "-C ";
            }

            if ((flags & AssemblyFlags.List) != 0)
            {
                this.flagsString += "-T ";
            }

            if ((flags & AssemblyFlags.Symtable) != 0)
            {
                this.flagsString += "-L ";
            }

            if ((flags & AssemblyFlags.Stats) != 0)
            {
                this.flagsString += "-S ";
            }

            if ((flags & AssemblyFlags.Normal) == 0)
            {
                this.flagsString += "-O ";
            }

            if ((flags & AssemblyFlags.Commandline) != 0)
            {
                this.flagsString += "-V ";
            }
        }

        public void SetInputFile(string file)
        {
            this.inputFile = file;
        }

        public void SetOutputFile(string file)
        {
            this.outputFile = file;
        }

        public void SetWorkingDirectory(string dir)
        {
            this.wabbitspasm.StartInfo.WorkingDirectory = dir;
        }
    }
}