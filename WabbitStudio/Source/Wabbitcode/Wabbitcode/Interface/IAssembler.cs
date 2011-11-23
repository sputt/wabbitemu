using System;

namespace Revsoft.Wabbitcode.Interface
{
	interface IAssembler
	{
		void AddDefine(string name, string value);
		void AddIncludeDir(string path);
		void ClearDefines();
		void ClearIncludeDirs();
		void SetInputFile(string file);
		void SetOutputFile(string file);
		string Assemble();
	}
}
