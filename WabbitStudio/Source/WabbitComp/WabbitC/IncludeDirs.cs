using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC
{
	public static class IncludeDirs
	{
		static List<string> localIncludes = new List<string>();
		static public List<string> LocalIncludes { get { return localIncludes; } }

		static List<string> sysIncludes = new List<string>();
		static public List<string> SystemIncludes { get { return sysIncludes; } }
	}
}
