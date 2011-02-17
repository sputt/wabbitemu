using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Revsoft.Wabbitcode.Services
{

    public class Assembler
    {
        static readonly Assembler instance = new Assembler();
        public static Assembler Instance { get { return instance; } }

        public Assembler()
        {

        }

        public void ShowMessage()
        {
            SPASMShowMessage();
        }

        public void SetInputFile(string file)
        {
            SPASMSetInputFile(file);
        }

        public void SetOutputFile(string file)
        {
            SPASMSetOutputFile(file);
        }

        public void ClearDefines()
        {
            SPASMClearDefines();
        }

        public void ClearIncludeDirs()
        {
            SPASMClearIncludes();
        }

        public void AddIncludeDir(string path)
        {
            SPASMAddInclude(path);
        }

        public void AddDefine(string name, string value)
        {
            SPASMAddDefine(name, value);
        }

        public string GetOutput()
        {
            return SPASMGetOutput();
        }

        public void Assemble()
        {
            SPASMRunAssembly();
        }

        #region SPASM Defines
        [DllImport("SPASM.dll", EntryPoint = "_ShowMessage@0")]
        static extern int SPASMShowMessage();

        [DllImport("SPASM.dll", EntryPoint = "_SetInputFile@4")]
        static extern int SPASMSetInputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport("SPASM.dll", EntryPoint = "_SetOutputFile@4")]
        static extern int SPASMSetOutputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport("SPASM.dll", EntryPoint = "_RunAssembly@0")]
        static extern int SPASMRunAssembly();

        //we done care about lpResult, make sure it is always NULL to avoid mem leak
        [DllImport("SPASM.dll", EntryPoint = "_RunAssemblyWithArguments@12")]
        static extern int SPASMRunAssemblyWithArguments([In, MarshalAs(UnmanagedType.LPStr)]
                                                            string szCommand, IntPtr lpResult, IntPtr cbResult);

        [DllImport("SPASM.dll", EntryPoint = "_ClearDefines@0")]
        static extern int SPASMClearDefines();

        [DllImport("SPASM.dll", EntryPoint = "_AddDefine@8")]
        static extern int SPASMAddDefine([In, MarshalAs(UnmanagedType.LPStr)] string name,
                                           [In, MarshalAs(UnmanagedType.LPStr)] string value);

        [DllImport("SPASM.dll", EntryPoint = "_ClearIncludes@0")]
        static extern int SPASMClearIncludes();

        [DllImport("SPASM.dll", EntryPoint = "_AddInclude@4")]
        static extern int SPASMAddInclude([In, MarshalAs(UnmanagedType.LPStr)] string directory);

        [DllImport("SPASM.dll", EntryPoint = "?SetMode@@YGXH@Z")]
        static extern int SPASMSetMode(Asm_Mode mode);

        [DllImport("SPASM.dll", EntryPoint = "_GetStdOut@0")]
        static extern string SPASMGetOutput();

        public enum Asm_Mode
        {
            Mode_Normal = 1,
            Mode_Code_Counter = 2,
            Mode_Symtable = 4,
            Mode_Stats = 8,
            Mode_List = 16,
            Mode_Commandline = 32,
        }
        #endregion
    }
}
