using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Utilities;

namespace Revsoft.Wabbitcode.Services
{
	public class AssemblerService : IAssemblerService
	{
		#region IService

		public void InitService(params Object[] objects)
		{
			Assembler = new Assembler();
		}

		public void DestroyService()
		{
			Assembler = null;
		}

		#endregion

		public Assembler Assembler { get; private set; }

		/// <summary>
		/// Assembles a file using SPASM and optionally sends it to an instance of Wabbitemu
		/// </summary>
		/// <param name="inputPath">Input file to send as the first param to spasm</param>
		/// <param name="outputPath">Output file for spasm to create</param>
		/// <param name="sendFileEmu">Indicates whether the file should be attempted to be sent to Wabbitemu</param>
		/// <returns>True if successful else false</returns>
		public bool AssembleFile(FilePath inputPath,
						FilePath outputPath, 
						IList<IDefine> defines = null,
						AssemblyFlags flags = AssemblyFlags.Assemble,
						Action<string> callback = null)
		{
			string originalDir = Path.GetDirectoryName(inputPath);
			Assembler.ClearIncludeDirs();
			Assembler.ClearDefines();
			Assembler.AddIncludeDir(originalDir);
			
			Assembler.SetInputFile(inputPath);
			Assembler.SetOutputFile(outputPath);

			//Assembler.SetFlags(SPASM.AssemblyFlags.MODE_NORMAL | AssemblyFlags.MODE_LIST);
			string output = Assembler.Assemble();

			if (callback != null)
			{
				callback(output);
			}
			return true;
		}
	}
}
