using System.Collections.Generic;
using System.Linq;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.DockingWindows.Tracking
{
    public class VariableDisplayManager
    {
        private static readonly VariableDisplayManager instance = new VariableDisplayManager();

        public static VariableDisplayManager Instance
        {
            get
            {
                return instance;
            }
        }

        static VariableDisplayManager()
        {
            ExpressionEvaluator evaluator = new ExpressionEvaluator();
            Instance.RegisterVariableDisplayController(new ByteDisplayController(evaluator));
            Instance.RegisterVariableDisplayController(new WordDisplayController(evaluator));
            Instance.RegisterVariableDisplayController(new StringVariableDisplayController(evaluator));
            Instance.RegisterVariableDisplayController(new GrayImageVariableDisplayController(evaluator));
            Instance.RegisterVariableDisplayController(new BlackAndWhiteImageVariableDisplayController(evaluator));
            Instance.RegisterVariableDisplayController(new LabelVariableDisplayController(evaluator));
        }

        private VariableDisplayManager()
        {
            // disallow instantiation
        }

        private readonly Dictionary<string, IVariableDisplayController> _controllerMap = new Dictionary<string, IVariableDisplayController>();
        public IVariableDisplayController DefaultController { get; private set; }

        public IEnumerable<string> ControllerNames
        {
            get { return _controllerMap.Keys.ToList(); }
        }

        public void RegisterVariableDisplayController(IVariableDisplayController controller)
        {
            if (DefaultController == null)
            {
                DefaultController = controller;
            }

            _controllerMap.Add(controller.Name, controller);
        }

        public IVariableDisplayController GetVariableDisplayController(string name)
        {
            IVariableDisplayController controller;
            return _controllerMap.TryGetValue(name, out controller) ? controller : null;
        }
    }
}