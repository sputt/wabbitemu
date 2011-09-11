using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Utilities;
using Revsoft.Wabbitcode.Interface;

namespace Revsoft.Wabbitcode.Services
{
	/// <summary>
	/// Enum for describing how spasm options
	/// </summary>
	public enum AssemblyFlags
	{
		/// <summary>
		/// Normal assembly. One file in, one file out
		/// Equivalent to:
		/// spasm inputPath outputPath
		/// </summary>
		Assemble,
		/// <summary>
		/// Assembles the file and also generates a list file useful for debugging
		/// -T flag
		/// </summary>
		List,
		/// <summary>
		/// Assembles the file and also generates a list of the addresses of labels.
		/// -L flag
		/// </summary>
		Label,
		/// <summary>
		/// Does not run pass 2, so no labels are resolved and no output files are generated.
		/// -O flag
		/// </summary>
		NoOutput,
		/// <summary>
		/// Makes spasm cAse sensitive when assembling
		/// -A flag
		/// </summary>
		CaseSensitive,

	}

	public interface IAssemblerService : IService
	{
		bool AssembleFile(FilePath inputPath,
						FilePath outputPath,
						IList<IDefine> defines = null,
						AssemblyFlags flags = AssemblyFlags.Assemble,
						Action<string> callback = null);
	}
}
