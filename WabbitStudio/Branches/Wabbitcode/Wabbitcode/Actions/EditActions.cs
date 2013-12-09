using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.GUI.Dialogs;
using Revsoft.Wabbitcode.GUI.DockingWindows;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.Actions
{
    public class CutAction : AbstractUiAction
    {
        private readonly IClipboardOperation _activeContent;

        public CutAction()
        {
            IDockingService dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _activeContent = dockingService.ActiveContent as IClipboardOperation;
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

        public CopyAction()
        {
            IDockingService dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _activeContent = dockingService.ActiveContent as IClipboardOperation;
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

        public PasteAction()
        {
            IDockingService dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _activeContent = dockingService.ActiveContent as IClipboardOperation;
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

        public SelectAllAction()
        {
            IDockingService dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _activeContent = dockingService.ActiveContent as ISelectable;
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

        public UndoAction()
        {
            IDockingService dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _activeContent = dockingService.ActiveContent as IUndoable;
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

        public RedoAction()
        {
            IDockingService dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _activeContent = dockingService.ActiveContent as IUndoable;
        }

        public override void Execute()
        {
            if (_activeContent != null)
            {
                _activeContent.Redo();
            }
        }
    }

    public class FindAction : AbstractUiAction
    {
        public override void Execute()
        {
            var dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            var activeTextEditor = dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.ShowFindForm(activeTextEditor.Parent as Form, SearchMode.Find);
        }
    }

    public class FindInFilesAction : AbstractUiAction
    {
        public override void Execute()
        {
            var dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            var activeTextEditor = dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                FindAndReplaceForm.Instance.ShowFor(null, null, SearchMode.FindInFiles);
            }
            else
            {
                activeTextEditor.ShowFindForm(activeTextEditor.Parent as Form, SearchMode.FindInFiles);
            }
        }
    }

    public class ReplaceAction : AbstractUiAction
    {
        public override void Execute()
        {
            var dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            var activeTextEditor = dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.ShowFindForm(activeTextEditor.Parent as Form, SearchMode.Replace);
        }
    }

    public class ReplaceInFilesAction : AbstractUiAction
    {
        public override void Execute()
        {
            var dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            var activeTextEditor = dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                FindAndReplaceForm.Instance.ShowFor(null, null, SearchMode.FindInFiles);
            }
            else
            {
                activeTextEditor.ShowFindForm(activeTextEditor.Parent as Form, SearchMode.FindInFiles);
            }
        }
    }

    public class FindAllReferencesAction : AbstractUiAction
    {
        private readonly ITextEditor _activeTextEditor;
        private readonly IDockingService _dockingService;
        private readonly IProjectService _projectService;
        private readonly FindResultsWindow _findResults;

        public FindAllReferencesAction()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            _projectService = ServiceFactory.Instance.GetServiceInstance<IProjectService>();
            _activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            _findResults = _dockingService.GetDockingWindow(FindResultsWindow.WindowIdentifier) as FindResultsWindow;
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

            _findResults.NewFindResults(word, _projectService.Project.ProjectName);
            var refs = _projectService.FindAllReferences(word);
            foreach (var fileRef in refs.SelectMany(reference => reference))
            {
                _findResults.AddFindResult(fileRef);
            }

            _findResults.DoneSearching();
            _dockingService.ShowDockPanel(_findResults);
        }
    }

    public class ToUpperAction : AbstractUiAction
    {
        public override void Execute()
        {
            IDockingService dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            var activeTextEditor = dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.SelectedTextToUpper();
        }
    }

    public class ToLowerAction : AbstractUiAction
    {
        public override void Execute()
        {
            IDockingService dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            var activeTextEditor = dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.SelectedTextToLower();
        }
    }

    public class InvertCaseAction : AbstractUiAction
    {
        public override void Execute()
        {
            IDockingService dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            var activeTextEditor = dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.SelectedTextInvertCase();
        }
    }

    public class ToSentenceCaseAction : AbstractUiAction
    {
        public override void Execute()
        {
            IDockingService dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
            var activeTextEditor = dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.SelectedTextToSentenceCase();
        }
    }

    public class FormatDocumentAction : AbstractUiAction
    {
        private readonly IDockingService _dockingService;

        public FormatDocumentAction()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
        }

        public override void Execute()
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.FormatLines();
        }
    }

    public class ConvertSpacesToTabsAction : AbstractUiAction
    {
        private readonly IDockingService _dockingService;

        public ConvertSpacesToTabsAction()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
        }

        public override void Execute()
        {
            var activeTextEditor = _dockingService.ActiveDocument as ITextEditor;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.ConvertSpacesToTabs();
        }
    }

    public class ToggleBookmark : AbstractUiAction
    {
        private readonly IDockingService _dockingService;

        public ToggleBookmark()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
        }

        public override void Execute()
        {
            var activeTextEditor = _dockingService.ActiveDocument as IBookmarkable;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.ToggleBookmark();
        }
    }

    public class GotoNextBookmark : AbstractUiAction
    {
        private readonly IDockingService _dockingService;

        public GotoNextBookmark()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
        }

        public override void Execute()
        {
            var activeTextEditor = _dockingService.ActiveDocument as IBookmarkable;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.GotoNextBookmark();
        }
    }

    public class GotoPreviousBookmark : AbstractUiAction
    {
        private readonly IDockingService _dockingService;

        public GotoPreviousBookmark()
        {
            _dockingService = ServiceFactory.Instance.GetServiceInstance<IDockingService>();
        }

        public override void Execute()
        {
            var activeTextEditor = _dockingService.ActiveDocument as IBookmarkable;
            if (activeTextEditor == null)
            {
                return;
            }

            activeTextEditor.GotoPrevBookmark();
        }
    }

    public class EditPreferencesAction : AbstractUiAction
    {
        public override void Execute()
        {
            using (Preferences prefs = new Preferences())
            {
                prefs.ShowDialog();
            }
        }
    }
}
