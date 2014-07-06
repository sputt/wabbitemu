namespace Revsoft.Wabbitcode.Interfaces
{
    public interface IUndoable
    {
        void Undo();
        void Redo();
    }
}