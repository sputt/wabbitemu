using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using WabbitemuLib;

namespace Revsoft.Wabbitcode.Services.Debugger
{
    public class CPage : IPage
    {
        public int Index
        {
            get;
            set;
        }

        public bool IsFlash
        {
            get;
            set;
        }

        public byte Read(ushort Address)
        {
            throw new NotImplementedException();
        }

        public void Write(ushort Address, byte Value)
        {
            throw new NotImplementedException();
        }

        public CPage(int page, bool isRam)
        {
            Index = page;
            IsFlash = !isRam;
        }

        public dynamic Read(ushort Address, [System.Runtime.InteropServices.OptionalAttribute]object ByteCount)
        {
            throw new NotImplementedException();
        }

        public void Write(ushort Address, object Value)
        {
            throw new NotImplementedException();
        }
    }
}
