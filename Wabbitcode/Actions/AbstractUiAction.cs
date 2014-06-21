using System;

namespace Revsoft.Wabbitcode.Actions
{
    public abstract class AbstractUiAction
    {
        public virtual bool IsEnabled { get; set; }

        public abstract void Execute();

        public static void RunCommand(AbstractUiAction action)
        {
            if (action == null)
            {
                throw new InvalidOperationException("Action was null");
            }

            action.Execute();
        }
    }
}