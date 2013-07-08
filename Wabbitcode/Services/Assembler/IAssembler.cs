namespace Revsoft.Wabbitcode.Services.Assembler
{
    using System;

    public interface IAssembler : IDisposable
    {
        void AddDefine(string name, string value);

        void AddIncludeDir(string path);

        string Assemble();

        void ClearDefines();

        void ClearIncludeDirs();

        void SetCaseSensitive(bool caseSensitive);

        void SetFlags(AssemblyFlags flags);

        void SetInputFile(string file);

        void SetOutputFile(string file);

        void SetWorkingDirectory(string file);
    }
}