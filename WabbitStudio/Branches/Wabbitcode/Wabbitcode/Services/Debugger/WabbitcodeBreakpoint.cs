using System;
using System.Collections;
using System.Collections.Generic;


using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services.Debugger
{
	public enum HitCountEnum
	{
		BreakAlways = 0,
		BreakEqualTo = 1,
		BreakMultipleOf = 2,
		BreakGreaterThanEqualTo = 4
	}

	public class BreakCondition
	{
		public int A { get; set; }
		public int B { get; set; }
		public int C { get; set; }
		public byte CFlag { get; set; }
		public int D { get; set; }
		public int E { get; set; }
		public int H { get; set; }
		public byte HFlag { get; set; }
		public int IX { get; set; }
		public int IY { get; set; }
		public int L { get; set; }
		public byte NFlag { get; set; }
		public byte PVFlag { get; set; }
		public byte SFlag { get; set; }
		public int SP { get; set; }
		public byte ZFlag { get; set; }
	}

	public class WabbitcodeBreakpoint
	{
		private readonly List<BreakCondition> _breakCondition = new List<BreakCondition>();

		public ushort Address { get; set; }
		public bool Enabled { get; set; }
		public string File { get; set; }
		public HitCountEnum HitCountCondition { get; set; }
		public int HitCountConditionNumber { get; set; }
		public List<BreakCondition> BreakConditions
		{
			get { return _breakCondition; }
		}
		public bool IsRam { get; set; }
		public int LineNumber;
		public int NumberOfTimesHit;
		public byte Page;
		public IBreakpoint WabbitemuBreakpoint;

		public WabbitcodeBreakpoint()
		{
			HitCountCondition = HitCountEnum.BreakAlways;
			HitCountConditionNumber = 0;
			Enabled = true;
		}

		public WabbitcodeBreakpoint(string file, int lineNumber) : this()
		{
			File = file;
			LineNumber = lineNumber;
		}

		public WabbitcodeBreakpoint(ushort address, byte page, bool isRam)
		{
			Address = address;
			Page = page;
			IsRam = isRam;
		}

		public static bool operator !=(WabbitcodeBreakpoint break1, WabbitcodeBreakpoint break2)
		{
			if ((object)break1 == null || (object)break2 == null)
			{
				return (object)break1 != null || (object)break2 != null;
			}
			return !break1.Equals(break2);
		}

		public static bool operator ==(WabbitcodeBreakpoint break1, WabbitcodeBreakpoint break2)
		{
			if ((object)break1 == null || (object)break2 == null)
			{
				return (object)break1 == null && (object)break2 == null;
			}

			return break1.Equals(break2);
		}

		public override bool Equals(object obj)
		{
			if (!(obj is WabbitcodeBreakpoint))
			{
				return false;
			}

			WabbitcodeBreakpoint break2 = obj as WabbitcodeBreakpoint;
			return (Address == break2.Address && Page == break2.Page && IsRam == break2.IsRam) ||
				   (string.Equals(File, break2.File, StringComparison.OrdinalIgnoreCase) && LineNumber == break2.LineNumber);
		}

		public bool EvalCondition(IZ80 cpu, BreakCondition condition)
		{
			bool isTrue = true;

			if (condition.A >= 0xFFFF)
			{
				isTrue &= (cpu.AF >> 16) == (ushort)(condition.A >> 8);
			}

			if (condition.B >= 0xFFFF)
			{
				isTrue &= (cpu.BC >> 16) == (ushort)(condition.B >> 8);
			}

			if (condition.C >= 0xFFFF)
			{
				isTrue &= (cpu.BC & 0xFF) == (ushort)(condition.C >> 16);
			}

			if (condition.D >= 0xFFFF)
			{
				isTrue &= (cpu.DE >> 16) == (ushort)(condition.D >> 8);
			}

			if (condition.E >= 0xFFFF)
			{
				isTrue &= (cpu.DE & 0xFF) == (ushort)(condition.E >> 16);
			}

			if (condition.H >= 0xFFFF)
			{
				isTrue &= cpu.HL >> 8 == (ushort)(condition.H >> 16);
			}

			if (condition.L >= 0xFFFF)
			{
				isTrue &= (cpu.HL & 0xFF) == (ushort)(condition.L >> 16);
			}

			if (condition.IX >= 0xFFFF)
			{
				isTrue &= cpu.IX == (ushort)condition.IX;
			}

			if (condition.IY >= 0xFFFF)
			{
				isTrue &= cpu.IY == (ushort)(condition.IY >> 16);
			}

			if (condition.SP >= 0xFFFF)
			{
				isTrue &= cpu.SP == (ushort)(condition.SP >> 16);
			}

			if (condition.CFlag > 2)
			{
				isTrue &= (cpu.AF & 1) == condition.CFlag;
			}

			if (condition.NFlag >= 2)
			{
				isTrue &= (cpu.AF & 2) == condition.NFlag;
			}

			if (condition.PVFlag >= 2)
			{
				isTrue &= (cpu.AF & 4) == condition.PVFlag;
			}

			if (condition.HFlag >= 2)
			{
				isTrue &= (cpu.AF & 16) == condition.HFlag;
			}

			if (condition.ZFlag >= 2)
			{
				isTrue &= (cpu.AF & 64) == condition.ZFlag;
			}

			if (condition.SFlag >= 2)
			{
				isTrue &= (cpu.AF & 128) == condition.SFlag;
			}

			return isTrue;
		}
	}
}