using System.Collections.Generic;
using Revsoft.Wabbitcode.GUI.DockingWindows.Tracking;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public class DebuggingStructure
    {
        public string Name { get; set; }

        public List<TreeStructureModel> Properties { get; private set; }

        public DebuggingStructure()
        {
            Properties = new List<TreeStructureModel>();
        }
    }
}