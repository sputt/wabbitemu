using System.Linq;
using Revsoft.Wabbitcode.Interface;
using Revsoft.Wabbitcode.Services.Debugger;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Actions
{
    public class ToggleBreakpointAction : AbstractUiAction
    {
        private readonly ITextEditor _editor;

        public ToggleBreakpointAction(ITextEditor editor)
        {
            _editor = editor;
        }

        public override void Execute()
        {
            string fileName = _editor.FileName;
            int lineNum = _editor.CaretLine;
            WabbitcodeBreakpoint breakpoint = WabbitcodeBreakpointManager.Breakpoints
                .SingleOrDefault(b => b.File == fileName && b.LineNumber == lineNum);
            if (breakpoint == null)
            {
                WabbitcodeBreakpointManager.AddBreakpoint(fileName, lineNum);
            }
            else
            {
                WabbitcodeBreakpointManager.RemoveBreakpoint(fileName, lineNum);
            }
        }
    }

    public class StartDebug : AbstractUiAction
    {
        public override void Execute()
        {
            throw new System.NotImplementedException();
        }
    }
}
