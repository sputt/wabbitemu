using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace Revsoft.Wabbitcode.Utilities
{
	internal class NativeMethods
	{
		#region SPASM Defines
		[DllImport("SPASM.dll", EntryPoint = "_ShowMessage@0")]
		internal static extern int SPASMShowMessage();

		[DllImport("SPASM.dll", EntryPoint = "_SetInputFile@4", BestFitMapping = false, ThrowOnUnmappableChar = true)]
		internal static extern int SPASMSetInputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

		[DllImport("SPASM.dll", EntryPoint = "_SetOutputFile@4", BestFitMapping = false, ThrowOnUnmappableChar = true)]
		internal static extern int SPASMSetOutputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

		[DllImport("SPASM.dll", EntryPoint = "_RunAssembly@0")]
		internal static extern int SPASMRunAssembly();

		//we don't care about lpResult, make sure it is always NULL to avoid mem leak
		[DllImport("SPASM.dll", EntryPoint = "_RunAssemblyWithArguments@12", BestFitMapping = false, ThrowOnUnmappableChar = true)]
		internal static extern int SPASMRunAssemblyWithArguments([In, MarshalAs(UnmanagedType.LPStr)]
															string szCommand, IntPtr lpResult, IntPtr cbResult);

		[DllImport("SPASM.dll", EntryPoint = "_ClearDefines@0")]
		internal static extern int SPASMClearDefines();

		[DllImport("SPASM.dll", EntryPoint = "_AddDefine@8", BestFitMapping = false, ThrowOnUnmappableChar = true)]
		internal static extern int SPASMAddDefine([In, MarshalAs(UnmanagedType.LPStr)] string name,
										   [In, MarshalAs(UnmanagedType.LPStr)] string value);

		[DllImport("SPASM.dll", EntryPoint = "_ClearIncludes@0")]
		internal static extern int SPASMClearIncludes();

		[DllImport("SPASM.dll", EntryPoint = "_AddInclude@4", BestFitMapping=false, ThrowOnUnmappableChar=true)]
		internal static extern int SPASMAddInclude([In, MarshalAs(UnmanagedType.LPStr)] string directory);

		[DllImport("SPASM.dll", EntryPoint = "?SetMode@@YGXH@Z")]
		internal static extern int SPASMSetMode(Asm_Mode mode);

		[DllImport("SPASM.dll", EntryPoint = "_GetStdOut@0")]
		internal static extern string SPASMGetOutput();

		internal enum Asm_Mode
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
