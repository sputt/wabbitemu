using System.Linq;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Actions
{
    public class CutAction : AbstractUiAction
    {
        private readonly IClipboardOperation _activeContent;

        public CutAction(IClipboardOperation activeContent)
        {
            _activeContent = activeContent;
        }

        public override void Execute()
        {
            if (_activeContent != null)
            {
                _activeContent.Cut();
            }
        }
    }

    public class CopyAction : AbstractUiAction
    {
        private readonly IClipboardOperation _activeContent;

        public CopyAction(IClipboardOperation activeContent)
        {
            _activeContent = activeContent;
        }

        public override void Execute()
        {
            if (_activeContent != null)
            {
                _activeContent.Copy();
            }
        }
    }

    public class PasteAction : AbstractUiAction
    {
        private readonly IClipboardOperation _activeContent;

        public PasteAction(IClipboardOperation activeContent)
        {
            _activeContent = activeContent;
        }

        public override void Execute()
        {
            if (_activeContent != null)
            {
                _activeContent.Paste();
            }
        }
    }

    public class SelectAllAction : AbstractUiAction
    {
        private readonly ISelectable _activeContent;

        public SelectAllAction(ISelectable activeContent)
        {
            _activeContent = activeContent;
        }

        public override void Execute()
        {
            if (_activeContent != null)
            {
                _activeContent.SelectAll();
            }
        }
    }

    public class UndoAction : AbstractUiAction
    {
        private readonly IUndoable _activeContent;

        public UndoAction(IUndoable activeContent)
        {
            _activeContent = activeContent;
        }

        public override void Execute()
        {
            if (_activeContent != null)
            {
                _activeContent.Undo();
            }
        }
    }

    public class RedoAction : AbstractUiAction
    {
        private readonly IUndoable _activeContent;

        public RedoAction(IUndoable activeContent)
        {
            _activeContent = activeContent;
        }

        public override void Execute()
        {
            if (_activeContent != null)
            {
                _activeContent.Redo();
            }
        }
    }

    public class FindAllReferencesCommand : AbstractUiAction
    {
        private readonly ITextEditor _activeTextEditor;
        private readonly IDockingService _dockingService;
        private readonly IProjectService _projectService;

        public FindAllReferencesCommand()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
            _activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
        }

        public override void Execute()
        {
            if (_activeTextEditor == null)
            {
                return;
            }

            string word = _activeTextEditor.GetWordAtCaret();
            if (string.IsNullOrEmpty(word))
            {
                return;
            }

            _dockingService.FindResults.NewFindResults(word, _projectService.Project.ProjectName);
            var refs = _projectService.FindAllReferences(word);
            foreach (var fileRef in refs.SelectMany(reference => reference))
            {
                _dockingService.FindResults.AddFindResult(fileRef);
            }

            _dockingService.FindResults.DoneSearching();
            _dockingService.ShowDockPanel(_dockingService.FindResults);
        }
    }
}
