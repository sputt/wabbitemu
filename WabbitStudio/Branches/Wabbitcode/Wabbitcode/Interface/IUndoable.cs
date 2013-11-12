namespace Revsoft.Wabbitcode.Interface
{
    public interface IUndoable
    {
        void Undo();
        void Redo();
    }
}