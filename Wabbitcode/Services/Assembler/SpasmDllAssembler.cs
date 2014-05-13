using System;

namespace Revsoft.Wabbitcode.Services.Assembler
{
    internal class SpasmDllAssembler : IAssembler
    {
        /*AssemblerClass Assembler = new AssemblerClass();
            string originalDir = filePath.Substring(0, filePath.LastIndexOf('\\'));
            //ShowMessage();
            Assembler.ClearIncludeDirectories();
            Assembler.ClearDefines();
            Assembler.AddIncludeDirectory(originalDir);
            //if the user has some include directories we need to format them
            if (Properties.Settings.Default.includeDir != "")
            {
                string[] dirs = Properties.Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                    if (dir != "")
                        Assembler.AddIncludeDirectory(dir);
            }
            //now we can set the args for spasm
            IStream pStream = Assembler.GetOutputStream();
            CStreamWrapper TestStream = new CStreamWrapper(pStream);
            StreamReader sr = new StreamReader(TestStream);

            Assembler.SetInputFile(filePath);
            Assembler.SetOutputFile(assembledName);

            Assembler.SetFlags(SPASM.AssemblyFlags.MODE_NORMAL | AssemblyFlags.MODE_LIST);
            //assemble that fucker
            Assembler.Assemble();

            StringBuilder builder = new StringBuilder();
            string line;
            do
            {
                line = sr.ReadLine();
                builder.Append(line);
                builder.Append("\n");
            } while (line != null);*/

        public void AddDefine(string name, string value)
        {
            throw new NotImplementedException();
        }

        public void AddIncludeDir(string path)
        {
            throw new NotImplementedException();
        }

        public string Assemble()
        {
            throw new NotImplementedException();
        }

        public string Assemble(string code)
        {
            throw new NotImplementedException();
        }

        public void ClearDefines()
        {
            throw new NotImplementedException();
        }

        public void ClearIncludeDirs()
        {
            throw new NotImplementedException();
        }

        public void Dispose()
        {
            throw new NotImplementedException();
        }

        public void SetCaseSensitive(bool caseSensitive)
        {
            throw new NotImplementedException();
        }

        public void SetFlags(AssemblyFlags flags)
        {
            throw new NotImplementedException();
        }

        public void SetInputFile(string file)
        {
            throw new NotImplementedException();
        }

        public void SetOutputFile(string file)
        {
            throw new NotImplementedException();
        }

        public void SetWorkingDirectory(string file)
        {
            throw new NotImplementedException();
        }
    }
}