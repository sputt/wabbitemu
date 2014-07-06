namespace Revsoft.Wabbitcode.Interfaces
{
    public interface IBookmarkable
    {
        void GotoNextBookmark();
        void GotoPrevBookmark();
        void ToggleBookmark();
    }
}