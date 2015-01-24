using System.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Assembler
{
    public sealed class SpasmExeAssembler : IAssembler
    {
        private const string Quote = "\"";

        private bool _caseSensitive;
        private readonly List<KeyValuePair<string, string>> _defines = new List<KeyValuePair<string, string>>();
        private readonly List<string> _includeDirs = new List<string>();
        private string _inputFile;
        private string _outputFile;
        private readonly Process _wabbitspasm;
        private bool _disposed;

        public SpasmExeAssembler()
        {
            // create two new processes to run
            // setup wabbitspasm to run silently
            _wabbitspasm = new Process
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
            _defines.Add(new KeyValuePair<string, string>(name, value));
        }

        public void AddIncludeDir(string path)
        {
            if (string.IsNullOrEmpty(path))
            {
                return;
            }

            _includeDirs.Add(path);
        }

        public string Assemble(AssemblyFlags flags)
        {
            string arguments = Quote + _inputFile + Quote + " " + Quote + _outputFile + Quote;
            return SetupRunAssemble(arguments, flags);
        }

        public string Assemble(string code, AssemblyFlags flags)
        {
            string arguments = Quote + code + Quote;
            return SetupRunAssemble(arguments, flags);
        }

        private string SetupRunAssemble(string arguments, AssemblyFlags flags)
        {
            string output = string.Empty;
            string includeDir = _includeDirs.Count > 0 ? "-I " : string.Empty;
            includeDir = _includeDirs.Aggregate(includeDir, (current, dir) => current + (Quote + dir + Quote + ";"));

            string caseString = _caseSensitive ? " -A " : " ";
            _wabbitspasm.StartInfo.Arguments = includeDir + caseString + GetFlags(flags) + arguments;
            _wabbitspasm.OutputDataReceived += (sender, e) => output += e.Data + Environment.NewLine;
            _wabbitspasm.ErrorDataReceived += (sender, e) => output += e.Data + Environment.NewLine;
            _wabbitspasm.Start();
            _wabbitspasm.BeginOutputReadLine();
            _wabbitspasm.BeginErrorReadLine();
            _wabbitspasm.WaitForExit();

            return output;
        }

        public void ClearDefines()
        {
            _defines.Clear();
        }

        public void ClearIncludeDirs()
        {
            _includeDirs.Clear();
        }

        public void Dispose()
        {
            Dispose(true);
        }

        private void Dispose(bool disposing)
        {
            if (!_disposed)
            {
                if (disposing)
                {
                    _wabbitspasm.Dispose();
                }
            }
            _disposed = true;
        }

        public void SetCaseSensitive(bool caseSensitive)
        {
            _caseSensitive = caseSensitive;
        }

        private static string GetFlags(AssemblyFlags flags)
        {
            string flagsString = string.Empty;
            if ((flags & AssemblyFlags.CodeCounter) != 0)
            {
                flagsString += "-C ";
            }

            if ((flags & AssemblyFlags.List) != 0)
            {
                flagsString += "-T ";
            }

            if ((flags & AssemblyFlags.SymbolTable) != 0)
            {
                flagsString += "-L ";
            }

            if ((flags & AssemblyFlags.Stats) != 0)
            {
                flagsString += "-S ";
            }

            if ((flags & AssemblyFlags.Normal) == 0)
            {
                flagsString += "-O ";
            }

            if ((flags & AssemblyFlags.Commandline) != 0)
            {
                flagsString += "-V ";
            }

            return flagsString;
        }

        public void SetInputFile(string file)
        {
            _inputFile = file;
        }

        public void SetOutputFile(string file)
        {
            _outputFile = file;
        }

        public void SetWorkingDirectory(string dir)
        {
            _wabbitspasm.StartInfo.WorkingDirectory = dir;
        }
    }
}