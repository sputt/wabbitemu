using System;
using System.Threading;
namespace Revsoft.Wabbitcode.Services.Assembler
{
    class SpasmComAssembler : IAssembler
    {
        private static SPASM.Z80Assembler _spasm;
        private static Thread _stdoutReader;
        private static string _accumulatedOutput = "";

        public SpasmComAssembler()
        {
            if (_spasm == null)
            {
                _stdoutReader = new Thread(readerThread);
                _stdoutReader.SetApartmentState(ApartmentState.MTA);
                _stdoutReader.Start();
            }
        }

        [MTAThread]
        private static void readerThread()
        {
            _spasm = new SPASM.Z80Assembler();
            try
            {
                while (true)
                {
                    _accumulatedOutput += _spasm.StdOut.ReadAll();
                }
            }
            catch (Exception e)
            {
            }
        }

        public void AddDefine(string name, string value)
        {
            lock (_spasm)
            {
                _spasm.Defines.Add(name, value);
            }
        }

        public void AddIncludeDir(string path)
        {
            lock (_spasm)
            {
                _spasm.IncludeDirectories.Add(path);
            }
        }

        public string Assemble()
        {
            lock (_spasm)
            {
                _accumulatedOutput = "";
                _spasm.Assemble();
                return _accumulatedOutput;
            }
        }

        public string Assemble(string code)
        {
            lock (_spasm)
            {
                _accumulatedOutput = "";
                _spasm.Assemble(code);
                return _accumulatedOutput;
            }
        }

        public void ClearDefines()
        {
            lock (_spasm)
            {
                _spasm.Defines.RemoveAll();
            }
        }

        public void ClearIncludeDirs()
        {
            lock (_spasm)
            {
                _spasm.IncludeDirectories.Clear();
            }
        }

        public void SetCaseSensitive(bool caseSensitive)
        {
            lock (_spasm)
            {
                _spasm.CaseSensitive = caseSensitive;
            }
        }

        public void SetFlags(AssemblyFlags flags)
        {
            lock (_spasm)
            {
                _spasm.Options = (uint) flags;
            }
        }

        public void SetInputFile(string file)
        {
            lock (_spasm)
            {
                _spasm.InputFile = file;
            }
        }

        public void SetOutputFile(string file)
        {
            lock (_spasm)
            {
                _spasm.OutputFile = file;
            }
        }

        public void SetWorkingDirectory(string file)
        {
            lock (_spasm)
            {
                _spasm.CurrentDirectory = file;
            }
        }

        public void Dispose()
        {
            _spasm = null;
            _stdoutReader.Interrupt();
            _stdoutReader.Join();
            _stdoutReader = null;
        }
    }
}
