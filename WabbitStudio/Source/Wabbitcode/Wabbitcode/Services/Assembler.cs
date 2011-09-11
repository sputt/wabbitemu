using Revsoft.Wabbitcode.Utilities;
using Revsoft.Wabbitcode.Interface;

namespace Revsoft.Wabbitcode.Services
{

	public class Assembler : IAssembler
	{
		public Assembler()
		{

		}

		public void ShowMessage()
		{
			NativeMethods.SPASMShowMessage();
		}

		public void SetInputFile(string file)
		{
			NativeMethods.SPASMSetInputFile(file);
		}

		public void SetOutputFile(string file)
		{
			NativeMethods.SPASMSetOutputFile(file);
		}

		public void ClearDefines()
		{
			NativeMethods.SPASMClearDefines();
		}

		public void ClearIncludeDirs()
		{
			NativeMethods.SPASMClearIncludes();
		}

		public void AddIncludeDir(string path)
		{
			NativeMethods.SPASMAddInclude(path);
		}

		public void AddDefine(string name, string value)
		{
			NativeMethods.SPASMAddDefine(name, value);
		}

		public string GetOutput()
		{
			return NativeMethods.SPASMGetOutput();
		}

		public void Assemble()
		{
			NativeMethods.SPASMRunAssembly();
		}
	}
}
