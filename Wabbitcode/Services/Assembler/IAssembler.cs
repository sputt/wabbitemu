namespace Revsoft.Wabbitcode.Services.Assembler
{
    using System;

    public interface IAssembler : IDisposable
    {
        void AddDefine(string name, string value);

        void AddIncludeDir(string path);

        string Assemble(AssemblyFlags flags);
        string Assemble(string code, AssemblyFlags flags);

        void ClearDefines();

        void ClearIncludeDirs();

        void SetCaseSensitive(bool caseSensitive);

        void SetInputFile(string file);

        void SetOutputFile(string file);

        void SetWorkingDirectory(string file);
    }
}