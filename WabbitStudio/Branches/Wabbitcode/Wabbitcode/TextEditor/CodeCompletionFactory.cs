using System.Collections.Generic;
using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.EditorExtensions;

namespace Revsoft.Wabbitcode.TextEditor
{
    [UsedImplicitly]
    public class CodeCompletionFactory : ICodeCompletionFactory
    {
        private static readonly Dictionary<string, List<ICodeCompletionBinding>> Bindings =
            new Dictionary<string, List<ICodeCompletionBinding>>();

        public void RegisterCodeCompletionBinding(string extension, ICodeCompletionBinding binding)
        {
            if (!Bindings.ContainsKey(extension))
            {
                Bindings.Add(extension, new List<ICodeCompletionBinding>());
            }

            Bindings[extension].Add(binding);
        }

        public IEnumerable<ICodeCompletionBinding> GetBindingForExtension(string extension)
        {
            List<ICodeCompletionBinding> bindings;
            Bindings.TryGetValue(extension, out bindings);
            return bindings ?? new List<ICodeCompletionBinding>();
        }
    }
}