using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Panels
{
    /// <summary>
    /// Virtual functions for panels
    /// </summary>
    public interface IWabbitcodePanel
    {
        void Cut();

        void Copy();

        void Paste();

        void Undo();

        void Redo();
    }
}

