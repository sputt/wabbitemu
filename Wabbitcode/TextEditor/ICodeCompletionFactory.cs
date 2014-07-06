using System.Collections.Generic;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    public interface ICodeCompletionFactory
    {
        void RegisterCodeCompletionBinding(string extension, ICodeCompletionBinding binding);

        IEnumerable<ICodeCompletionBinding> GetBindingForExtension(string extension);
    }
}