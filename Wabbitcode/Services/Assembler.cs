using Revsoft.Wabbitcode.Utilities;
using Revsoft.Wabbitcode.Interface;
using System.Runtime.InteropServices;
using System;
using System.Collections.Generic;

namespace Revsoft.Wabbitcode.Services
{
	/// <summary>
	/// This is implemented in a way so that you only need to access the DLL while assembling
	/// </summary>
	public class Assembler : IAssembler
	{
		string inputFile;
		string outputFile;
		bool clearDefines;
		bool clearIncludeDirs;
		List<string> includeDirs;
		Dictionary<string, string> defines;

		public Assembler()
		{
			includeDirs = new List<string>();
			defines = new Dictionary<string, string>();
		}

		public void SetInputFile(string file)
		{
			inputFile = file;
		}

		public void SetOutputFile(string file)
		{
			outputFile = file;
		}

		public void ClearDefines()
		{
			clearDefines = true;
		}

		public void ClearIncludeDirs()
		{
			clearIncludeDirs = true;	
		}

		public void AddIncludeDir(string path)
		{
			includeDirs.Add(path);
		}

		public void AddDefine(string name, string value)
		{
			defines.Add(name, value);
		}

		public string Assemble()
		{
			NativeMethods.SPASMSetInputFile(inputFile);
			NativeMethods.SPASMSetOutputFile(outputFile);
			if (clearDefines)
			{
				NativeMethods.SPASMClearDefines();
				clearDefines = false;
			}
			if (clearIncludeDirs)
			{
				NativeMethods.SPASMClearIncludes();
				clearIncludeDirs = false;
			}

			foreach (var include in includeDirs)
			{
				NativeMethods.SPASMAddInclude(include);
			}
			includeDirs.Clear();

			foreach (var define in defines)
			{
				NativeMethods.SPASMAddDefine(define.Key, define.Value);
			}
			defines.Clear();

			NativeMethods.SPASMRunAssembly();

			IntPtr outputPtr = NativeMethods.SPASMGetOutput();
			return Marshal.PtrToStringAnsi(outputPtr);
		}
	}
}
