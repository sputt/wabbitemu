using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services.Assembler
{
    class SpasmComAssembler : IAssembler
    {
        private static SPASM.Z80Assembler spasm = null;

        public SpasmComAssembler()
        {
            lock (this)
            {
                if (spasm == null)
                {
                    spasm = new SPASM.Z80Assembler();
                }
            }
        }

        public void AddDefine(string name, string value)
        {
            spasm.Defines.Add(name, value);
        }

        public void AddIncludeDir(string path)
        {
            spasm.IncludeDirectories.Add(path);
        }

        public string Assemble()
        {
            spasm.Assemble();
            return spasm.StdOut.ReadAll();
        }

        public string Assemble(string code)
        {
            spasm.Assemble(code);
            return "DONE";
           // return spasm.StdOut.ReadAll();
        }

        public void ClearDefines()
        {
            spasm.Defines.RemoveAll();
        }

        public void ClearIncludeDirs()
        {
            spasm.IncludeDirectories.Clear();
        }

        public void SetCaseSensitive(bool caseSensitive)
        {
           // throw new NotImplementedException();
        }

        public void SetFlags(AssemblyFlags flags)
        {
            spasm.Options = (uint) flags;
        }

        public void SetInputFile(string file)
        {
            spasm.InputFile = file;
        }

        public void SetOutputFile(string file)
        {
            spasm.OutputFile = file;
        }

        public void SetWorkingDirectory(string file)
        {
            spasm.CurrentDirectory = file;
        }

        public void Dispose()
        {
            spasm = null;
        }
    }
}
