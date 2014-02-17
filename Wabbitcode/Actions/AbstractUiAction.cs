namespace Revsoft.Wabbitcode.Actions
{
    public abstract class AbstractUiAction
    {
        public virtual bool IsEnabled { get; set; }

        public abstract void Execute();

        public static void RunCommand(AbstractUiAction action)
        {
            action.Execute();
        }
    }
}
