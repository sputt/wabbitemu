using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Revsoft.Wabbitcode.GUI.DockingWindows;

namespace Revsoft.Wabbitcode.GUI.Dialogs
{
    public class TreeStructureModel
    {
        public string Name { get; set; }
        public int Size { get; set; }
        public VariableDisplayMethod Type { get; set; }
    }
}
