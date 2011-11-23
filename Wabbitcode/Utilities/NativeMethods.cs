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
		[DllImport("SPASM.dll", EntryPoint = "ShowMessage")]
		internal static extern int SPASMShowMessage();

		[DllImport("SPASM.dll", EntryPoint = "SetInputFile", BestFitMapping = false, ThrowOnUnmappableChar = true)]
		internal static extern int SPASMSetInputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

		[DllImport("SPASM.dll", EntryPoint = "SetOutputFile", BestFitMapping = false, ThrowOnUnmappableChar = true)]
		internal static extern int SPASMSetOutputFile([In, MarshalAs(UnmanagedType.LPStr)] string name);

		[DllImport("SPASM.dll", EntryPoint = "RunAssembly")]
		internal static extern int SPASMRunAssembly();

		//we don't care about lpResult, make sure it is always NULL to avoid mem leak
		[DllImport("SPASM.dll", EntryPoint = "RunAssemblyWithArguments", BestFitMapping = false, ThrowOnUnmappableChar = true)]
		internal static extern int SPASMRunAssemblyWithArguments([In, MarshalAs(UnmanagedType.LPStr)]
															string szCommand, IntPtr lpResult, IntPtr cbResult);

		[DllImport("SPASM.dll", EntryPoint = "ClearDefines")]
		internal static extern int SPASMClearDefines();

		[DllImport("SPASM.dll", EntryPoint = "AddDefine", BestFitMapping = false, ThrowOnUnmappableChar = true)]
		internal static extern int SPASMAddDefine([In, MarshalAs(UnmanagedType.LPStr)] string name,
										   [In, MarshalAs(UnmanagedType.LPStr)] string value);

		[DllImport("SPASM.dll", EntryPoint = "ClearIncludes")]
		internal static extern int SPASMClearIncludes();

		[DllImport("SPASM.dll", EntryPoint = "AddInclude", BestFitMapping=false, ThrowOnUnmappableChar=true)]
		internal static extern int SPASMAddInclude([In, MarshalAs(UnmanagedType.LPStr)] string directory);

		[DllImport("SPASM.dll", EntryPoint = "SetMode")]
		internal static extern int SPASMSetMode(Asm_Mode mode);

		[DllImport("SPASM.dll", EntryPoint = "GetStdOut")]
		internal static extern IntPtr SPASMGetOutput();

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
