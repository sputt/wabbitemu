using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Actions;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.GUI.Menus
{
    public class EditorContextMenu : ContextMenu
    {
        private readonly MenuItem _cutContext = new MenuItem("Cut");
        private readonly MenuItem _copyContext = new MenuItem("Copy");
        private readonly MenuItem _pasteContext = new MenuItem("Paste");
        private readonly MenuItem _separator1 = new MenuItem("-");
        private readonly MenuItem _renameContext = new MenuItem("Rename");
        private readonly MenuItem _extractMethodContext = new MenuItem("Extract Method");
        private readonly MenuItem _findRefContext = new MenuItem("Find References");
        private readonly MenuItem _separator2 = new MenuItem("-");
        private readonly MenuItem _selectAllContext = new MenuItem("Select All");
        private readonly MenuItem _setNextStateMenuItem = new MenuItem("Set Next Statement");
        private readonly MenuItem _fixCaseContext = new MenuItem("Fix Case");
        private readonly MenuItem _gotoButton = new MenuItem("Goto");

        private readonly WabbitcodeTextEditor _editor;
        private readonly IDebuggerService _debuggerService;
        private readonly IParserService _parserService;
        private readonly IProjectService _projectService;

        public EditorContextMenu(WabbitcodeTextEditor editor, IDebuggerService debuggerService,
            IParserService parserService, IProjectService projectService)
        {
            var refactorContext = new MenuItem("Refactor", new[] { _renameContext, _extractMethodContext });
            MenuItems.AddRange(new[]
            {
                _cutContext,
                _copyContext,
                _pasteContext,
                _separator1,
                refactorContext,
                _findRefContext,
                _separator2,
                _selectAllContext,
                _setNextStateMenuItem,
                _fixCaseContext,
                _gotoButton
            });

            _cutContext.Click += cutContext_Click;
            _copyContext.Click += copyContext_Click;
            _pasteContext.Click += pasteContext_Click;
            _renameContext.Click += renameContext_Click;
            _extractMethodContext.Click += extractMethodContext_Click;
            _findRefContext.Click += findRefContext_Click;
            _selectAllContext.Click += selectAllContext_Click;
            _setNextStateMenuItem.Click += setNextStateMenuItem_Click;
            _fixCaseContext.Click += fixCaseContext_Click;
            _gotoButton.Click += gotoButton_Click;

            Popup += contextMenu_Popup;
            
            _editor = editor;
            _debuggerService = debuggerService;
            _parserService = parserService;
            _projectService = projectService;
        }

        private void contextMenu_Popup(object sender, EventArgs e)
        {
            bool hasSelection = _editor.ActiveTextAreaControl.SelectionManager.HasSomethingSelected;
            _cutContext.Enabled = hasSelection;
            _copyContext.Enabled = hasSelection;
            _renameContext.Enabled = !hasSelection;
            _extractMethodContext.Enabled = hasSelection;

            bool isDebugging = _debuggerService.CurrentDebugger != null;
            _setNextStateMenuItem.Visible = isDebugging;

            if (string.IsNullOrEmpty(_editor.Text))
            {
                return;
            }

            int lineNum = _editor.ActiveTextAreaControl.Caret.Line;
            _fixCaseContext.Visible = false;
            _fixCaseContext.MenuItems.Clear();

            string line = _editor.GetLineText(lineNum);
            Match match = Regex.Match(line, "#include ((\"(?<includeFile>.*)(?<paren>\\)?)\")|((?!\")(?<includeFile>.*(?!\"))(?<paren>\\)?)))");
            bool isInclude = match.Success;

            var caret = _editor.ActiveTextAreaControl.Caret;
            var segment = _editor.Document.GetLineSegment(caret.Line);
            var word = segment.GetWord(caret.Column);
            string text = word == null ? string.Empty : word.Word;
            if (word != null && !string.IsNullOrEmpty(text) && !isInclude)
            {
                IEnumerable<IParserData> parserData = _parserService.GetParserData(text, Settings.Default.CaseSensitive).ToList();
                if (parserData.Any())
                {
                    foreach (IParserData data in parserData.Where(data => data.Name != text))
                    {
                        _fixCaseContext.Visible = true;
                        MenuItem item = new MenuItem(data.Name, fixCaseContext_Click);
                        _fixCaseContext.MenuItems.Add(item);
                    }
                    _gotoButton.Enabled = true;
                }
                else
                {
                    _gotoButton.Enabled = false;
                }
            }

            string gotoLabel = isInclude ? match.Groups["includeFile"].Value.Replace('"', ' ').Trim() : text;
            if (gotoLabel == "_")
            {
                match = Regex.Match(line, "(?<offset>(\\+|\\-)*)_");
                gotoLabel = match.Groups["offset"].Value + gotoLabel;
            }

            int num;
            if (!int.TryParse(gotoLabel, out num))
            {
                if (isInclude)
                {
                    bool exists = Path.IsPathRooted(gotoLabel) ? File.Exists(gotoLabel) : FindFileIncludes(gotoLabel);
                    if (exists)
                    {
                        _gotoButton.Text = "Open " + gotoLabel;
                        _gotoButton.Enabled = true;
                    }
                    else
                    {
                        _gotoButton.Text = "File " + gotoLabel + " doesn't exist";
                        _gotoButton.Enabled = false;
                    }
                }
                else
                {
                    if (_gotoButton.Enabled)
                    {
                        _gotoButton.Text = "Goto " + gotoLabel;
                        _gotoButton.Enabled = !string.IsNullOrEmpty(gotoLabel);
                    }
                    else
                    {
                        _gotoButton.Text = "Unable to find " + gotoLabel;
                    }
                }
            }
            else
            {
                _gotoButton.Text = "Goto ";
                _gotoButton.Enabled = false;
            }
        }

        private void cutContext_Click(object sender, EventArgs e)
        {
            _editor.Cut();
        }

        private void copyContext_Click(object sender, EventArgs e)
        {
            _editor.Copy();
        }

        private void pasteContext_Click(object sender, EventArgs e)
        {
            _editor.Paste();
        }

        private void selectAllContext_Click(object sender, EventArgs e)
        {
            _editor.SelectAll();
        }

        private void setNextStateMenuItem_Click(object sender, EventArgs e)
        {
            FilePath filePath = new FilePath(_editor.FileName);
            int line = _editor.ActiveTextAreaControl.Caret.Line + 1;
            _debuggerService.CurrentDebugger.SetPCToSelect(filePath, line);
        }

        private void gotoButton_Click(object sender, EventArgs e)
        {
            // no need to make a stricter regex, as we own the inputs here
            Match match = Regex.Match(_gotoButton.Text, "(?<action>.*?) (?<name>.*)");
            string action = match.Groups["action"].Value;
            FilePath text = new FilePath(match.Groups["name"].Value);

            if (action == "Goto")
            {
                FilePath filePath = new FilePath(_editor.FileName);
                int line = _editor.ActiveTextAreaControl.Caret.Line;
                AbstractUiAction.RunCommand(new GotoDefinitionAction(filePath, text, line));
            }
            else
            {
                FilePath fileFullPath = Path.IsPathRooted(text) ? text :
                    _projectService.Project.GetFilePathFromRelativePath(text).NormalizePath();
                AbstractUiAction.RunCommand(new GotoFileAction(fileFullPath));
            }
        }

        private static void fixCaseContext_Click(object sender, EventArgs e)
        {
            MenuItem item = sender as MenuItem;
            if (item == null)
            {
                return;
            }

            AbstractUiAction.RunCommand(new FixCaseAction(item.Text));
        }

        private static void findRefContext_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new FindAllReferencesAction());
        }

        private static void renameContext_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new RefactorRenameAction());
        }

        private static void extractMethodContext_Click(object sender, EventArgs e)
        {
            AbstractUiAction.RunCommand(new RefactorExtractMethodAction());
        }

        private bool FindFileIncludes(string gotoLabel)
        {
            return !string.IsNullOrEmpty(_projectService.Project.GetFilePathFromRelativePath(gotoLabel));
        }
    }
}
