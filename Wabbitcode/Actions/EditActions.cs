using Revsoft.Wabbitcode.Interface;

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
}
