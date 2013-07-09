using System;
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

    public struct BreakCondition
    {
        public int a;
        public int b;
        public int c;
        public byte cFlag;
        public int d;
        public int e;
        public int h;
        public byte hFlag;
        public int ix;
        public int iy;
        public int l;
        public byte nFlag;
        public byte pvFlag;
        public byte sFlag;
        public int sp;
        public byte zFlag;

        // public int address;
        // public byte value;
    }

    public class WabbitcodeBreakpoint
    {
        public ushort Address;
        public List<BreakCondition> breakCondition = new List<BreakCondition>();
        public bool Enabled = true;
        public string file;
        public HitCountEnum hitCountCondition = HitCountEnum.BreakAlways;
        public int hitCountConditionNumber;
        public bool IsRam;
        public int lineNumber;
        public int numberOfTimesHit;
        public byte Page;
        public IBreakpoint WabbitemuBreakpoint;

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
            this.Address = address;
            this.Page = page;
            this.IsRam = isRam;
        }

        public static bool operator !=(WabbitcodeBreakpoint break1, WabbitcodeBreakpoint break2)
        {
            if ((object)break1 == null || (object)break2 == null)
            {
                if ((object)break1 == null && (object)break2 == null)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
            return (break1.Address != break2.Address || break1.Page != break2.Page || break1.IsRam != break2.IsRam) &&
                   (break1.file != break2.file || break1.lineNumber != break2.lineNumber);
        }

        public static bool operator ==(WabbitcodeBreakpoint break1, WabbitcodeBreakpoint break2)
        {
            if ((object)break1 == null || (object)break2 == null)
            {
                if ((object)break1 == null && (object)break2 == null)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }

            return (break1.Address == break2.Address && break1.Page == break2.Page && break1.IsRam == break2.IsRam) ||
                   (break1.file == break2.file && break1.lineNumber == break2.lineNumber);
        }

        public override bool Equals(object obj)
        {
            if (!(obj is WabbitcodeBreakpoint))
            {
                return false;
            }

            WabbitcodeBreakpoint break2 = obj as WabbitcodeBreakpoint;
            return (this.Address == break2.Address && this.Page == break2.Page && this.IsRam == break2.IsRam) ||
                   (string.Equals(this.file, break2.file, StringComparison.OrdinalIgnoreCase) && this.lineNumber == break2.lineNumber);
        }

        public bool EvalCondition(IZ80 cpu, BreakCondition condition)
        {
            bool isTrue = true;

            if (condition.a >= 0xFFFF)
            {
                isTrue &= (cpu.AF >> 16) == (ushort)(condition.a >> 8);
            }

            if (condition.b >= 0xFFFF)
            {
                isTrue &= (cpu.BC >> 16) == (ushort)(condition.b >> 8);
            }

            if (condition.c >= 0xFFFF)
            {
                isTrue &= (cpu.BC & 0xFF) == (ushort)(condition.c >> 16);
            }

            if (condition.d >= 0xFFFF)
            {
                isTrue &= (cpu.DE >> 16) == (ushort)(condition.d >> 8);
            }

            if (condition.e >= 0xFFFF)
            {
                isTrue &= (cpu.DE & 0xFF) == (ushort)(condition.e >> 16);
            }

            if (condition.h >= 0xFFFF)
            {
                isTrue &= cpu.HL >> 8 == (ushort)(condition.h >> 16);
            }

            if (condition.l >= 0xFFFF)
            {
                isTrue &= (cpu.HL & 0xFF) == (ushort)(condition.l >> 16);
            }

            if (condition.ix >= 0xFFFF)
            {
                isTrue &= cpu.IX == (ushort)condition.ix;
            }

            if (condition.iy >= 0xFFFF)
            {
                isTrue &= cpu.IY == (ushort)(condition.iy >> 16);
            }

            if (condition.sp >= 0xFFFF)
            {
                isTrue &= cpu.SP == (ushort)(condition.sp >> 16);
            }

            if (condition.cFlag > 2)
            {
                isTrue &= (cpu.AF & 1) == condition.cFlag;
            }

            if (condition.nFlag >= 2)
            {
                isTrue &= (cpu.AF & 2) == condition.nFlag;
            }

            if (condition.pvFlag >= 2)
            {
                isTrue &= (cpu.AF & 4) == condition.pvFlag;
            }

            if (condition.hFlag >= 2)
            {
                isTrue &= (cpu.AF & 16) == condition.hFlag;
            }

            if (condition.zFlag >= 2)
            {
                isTrue &= (cpu.AF & 64) == condition.zFlag;
            }

            if (condition.sFlag >= 2)
            {
                isTrue &= (cpu.AF & 128) == condition.sFlag;
            }

            return isTrue;
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }
    }
}