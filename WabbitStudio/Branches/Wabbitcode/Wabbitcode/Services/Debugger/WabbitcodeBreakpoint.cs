using System;
using System.Collections.Generic;
using System.Text;
using Revsoft.Wabbitcode.Services;
#if NEW_DEBUGGING
using WabbitemuLib;
#endif

namespace Revsoft.Wabbitcode.Classes
{
	public class WabbitcodeBreakpoint
	{
		public bool Enabled = true;
		public string file;
		public int lineNumber;
		public ushort Address;
		public byte Page;
		public bool IsRam;
		public int numberOfTimesHit;
		public int hitCountConditionNumber;
		public HitCountEnum hitCountCondition = HitCountEnum.BreakAlways;
		public List<BreakCondition> breakCondition = new List<BreakCondition>();

		public WabbitcodeBreakpoint()
		{

		}

		public WabbitcodeBreakpoint(string file, int lineNumber)
		{
			this.file = file;
			this.lineNumber = lineNumber;
		}

		public WabbitcodeBreakpoint(ushort address, byte page, bool isRam)
		{
			Address = address;
			Page = page;
			IsRam = isRam;
		}

		public override bool Equals(object obj)
		{
			if (obj.GetType() != typeof(WabbitcodeBreakpoint))
				return false;
			WabbitcodeBreakpoint break2 = obj as WabbitcodeBreakpoint;
			return (Address == break2.Address && Page == break2.Page && IsRam == break2.IsRam) ||
					(file == break2.file && lineNumber == break2.lineNumber);
		}

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

		public static bool operator ==(WabbitcodeBreakpoint break1, WabbitcodeBreakpoint break2)
		{
			if ((object)break1 == null || (object)break2 == null)
				if ((object)break1 == null && (object)break2 == null)
					return true;
				else
					return false;
			return (break1.Address == break2.Address && break1.Page == break2.Page && break1.IsRam == break2.IsRam) ||
							(break1.file == break2.file && break1.lineNumber == break2.lineNumber);
		}

		public static bool operator !=(WabbitcodeBreakpoint break1, WabbitcodeBreakpoint break2)
		{
			if ((object) break1 == null || (object) break2 == null)
				if ((object) break1 == null && (object) break2 == null)
					return false;
				else
					return true;
			return (break1.Address != break2.Address || break1.Page != break2.Page || break1.IsRam != break2.IsRam) &&
					(break1.file != break2.file || break1.lineNumber != break2.lineNumber);
		}

		public bool EvalCondition(BreakCondition condition)
		{
			bool isTrue = true;
#if NEW_DEBUGGING
            IZ80 state = DebuggerService.Debugger.CPU;
#else
			CWabbitemu.Z80_State state = DebuggerService.Debugger.getState();
#endif
			if (condition.a >= 0xFFFF)
				isTrue &= ((state.AF >> 16) == (ushort)(condition.a >> 8));
			if (condition.b >= 0xFFFF)
				isTrue &= ((state.BC >> 16) == (ushort)(condition.b >> 8));
			if (condition.c >= 0xFFFF)
				isTrue &= ((state.BC & 0xFF) == (ushort)(condition.c >> 16));
			if (condition.d >= 0xFFFF)
				isTrue &= ((state.DE >> 16) == (ushort)(condition.d >> 8));
			if (condition.e >= 0xFFFF)
				isTrue &= ((state.DE & 0xFF) == (ushort)(condition.e >> 16));
			if (condition.h >= 0xFFFF)
				isTrue &= (state.HL >> 8 == (ushort)(condition.h >> 16));
			if (condition.l >= 0xFFFF)
				isTrue &= ((state.HL & 0xFF) == (ushort)(condition.l >> 16));
			if (condition.ix >= 0xFFFF)
				isTrue &= (state.IX == (ushort)condition.ix);
			if (condition.iy >= 0xFFFF)
				isTrue &= (state.IY == (ushort)(condition.iy >> 16));
			if (condition.sp >= 0xFFFF)
				isTrue &= (state.SP == (ushort)(condition.sp >> 16));
			if (condition.cFlag > 2)
				isTrue &= ((state.AF & 1) == condition.cFlag);
			if (condition.nFlag >= 2)
				isTrue &= ((state.AF & 2) == condition.nFlag);
			if (condition.pvFlag >= 2)
				isTrue &= ((state.AF & 4) == condition.pvFlag);
			if (condition.hFlag >= 2)
				isTrue &= ((state.AF & 16) == condition.hFlag);
			if (condition.zFlag >= 2)
				isTrue &= ((state.AF & 64) == condition.zFlag);
			if (condition.sFlag >= 2)
				isTrue &= ((state.AF & 128) == condition.sFlag);
			return isTrue;
		}
	}

	public enum HitCountEnum
	{
		BreakAlways = 0,
		BreakEqualTo = 1,
		BreakMultipleOf = 2,
		BreakGreaterThanEqualTo = 4
	}

	public struct BreakCondition
	{
		public int a;
		public int b;
		public int c;
		public int d;
		public int e;
		public int h;
		public int l;
		public int ix;
		public int iy;
		public int sp;
		public byte zFlag;
		public byte cFlag;
		public byte sFlag;
		public byte hFlag;
		public byte nFlag;
		public byte pvFlag;
		//public int address;
		//public byte value;
	}
}
