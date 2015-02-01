using Aga.Controls.Tree;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    public class TreeStructureModel : Node
    {
        private readonly VariableDisplayManager _displayManager = VariableDisplayManager.Instance;

        public string Name { get; set; }
        public int Size { get; set; }
        public string Type { get; set; }
        public int Offset { get; set; }

        internal IVariableDisplayController Controller
        {
            get { return _displayManager.GetVariableDisplayController(Type); }
        }
    }
}
