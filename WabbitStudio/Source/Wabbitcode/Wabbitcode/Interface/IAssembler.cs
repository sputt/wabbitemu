using System;

namespace Revsoft.Wabbitcode.Interface
{
	interface IAssembler
	{
		void AddDefine(string name, string value);
		void AddIncludeDir(string path);
		void Assemble();
		void ClearDefines();
		void ClearIncludeDirs();
		string GetOutput();
		void SetInputFile(string file);
		void SetOutputFile(string file);
	}
}
