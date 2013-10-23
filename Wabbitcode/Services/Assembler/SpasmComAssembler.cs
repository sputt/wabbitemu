namespace Revsoft.Wabbitcode.Services.Assembler
{
    class SpasmComAssembler : IAssembler
    {
        private static SPASM.Z80Assembler _spasm;
        private static readonly object SPASMLock = new object();

        public SpasmComAssembler()
        {
            lock (SPASMLock)
            {
                if (_spasm == null)
                {
                    _spasm = new SPASM.Z80Assembler();
                }
            }
        }

        public void AddDefine(string name, string value)
        {
            _spasm.Defines.Add(name, value);
        }

        public void AddIncludeDir(string path)
        {
            _spasm.IncludeDirectories.Add(path);
        }

        public string Assemble()
        {
            _spasm.Assemble();
            return _spasm.StdOut.ReadAll();
        }

        public string Assemble(string code)
        {
            _spasm.Assemble(code);
           return _spasm.StdOut.ReadAll();
        }

        public void ClearDefines()
        {
            _spasm.Defines.RemoveAll();
        }

        public void ClearIncludeDirs()
        {
            _spasm.IncludeDirectories.Clear();
        }

        public void SetCaseSensitive(bool caseSensitive)
        {
            _spasm.CaseSensitive = caseSensitive;
        }

        public void SetFlags(AssemblyFlags flags)
        {
            _spasm.Options = (uint) flags;
        }

        public void SetInputFile(string file)
        {
            _spasm.InputFile = file;
        }

        public void SetOutputFile(string file)
        {
            _spasm.OutputFile = file;
        }

        public void SetWorkingDirectory(string file)
        {
            _spasm.CurrentDirectory = file;
        }

        public void Dispose()
        {
            _spasm = null;
        }
    }
}
