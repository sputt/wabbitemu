using System.Collections.Generic;
using Revsoft.Wabbitcode.TextEditor;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    public interface ICodeCompletionFactory
    {
        void RegisterCodeCompletionBinding(string extension, ICodeCompletionBinding binding);

        IEnumerable<ICodeCompletionBinding> GetBindingForExtension(string extension);
    }
}