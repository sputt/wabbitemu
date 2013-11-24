namespace Revsoft.Wabbitcode.Interface
{
    public interface IBookmarkable
    {
        void GotoNextBookmark();
        void GotoPrevBookmark();
        void ToggleBookmark();
    }
}