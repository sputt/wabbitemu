using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.EditorExtensions.Markers;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    internal class ErrorUnderliner : IDisposable
    {
        private readonly WabbitcodeTextEditor _editor;
        private readonly IDocument _document;

        public bool Enabled { get; set; }

        public ErrorUnderliner(WabbitcodeTextEditor editor)
        {
            _editor = editor;
            _document = _editor.Document;

            IBackgroundAssemblerService backgroundAssemblerService = DependencyFactory.Resolve<IBackgroundAssemblerService>();
            backgroundAssemblerService.BackgroundAssemblerComplete += BackgroundAssemblerService_BackgroundAssemblerComplete;
        }

        private void BackgroundAssemblerService_BackgroundAssemblerComplete(object sender, AssemblyFinishEventArgs e)
        {
            UnderlineErrors(e.Output.ParsedErrors);
        }

        private void UnderlineErrors(IEnumerable<BuildError> parsedErrors)
        {
            if (_editor.IsDisposed || _editor.Disposing || !Enabled)
            {
                return;
            }

            if (_editor.InvokeRequired)
            {
                _editor.Invoke(() => UnderlineErrors(parsedErrors));
                return;
            }

            _document.MarkerStrategy.RemoveAll(m => m is ErrorMarker);
            var options = Settings.Default.CaseSensitive ?
                StringComparison.Ordinal :
                StringComparison.OrdinalIgnoreCase;
            foreach (BuildError error in parsedErrors.Where(
                error => error.File == _editor.FileName))
            {
                var segment = _document.GetLineSegment(error.LineNumber - 1);
                Match match = Regex.Match(error.Description, "'(?<error>.*?)'");

                TextWord word;
                int offset = segment.Offset;
                int length;
                if (match.Success)
                {
                    word = segment.Words.FirstOrDefault(
                        s => s.Word.Equals(match.Groups["error"].Value, options)) ??
                           segment.Words.FirstOrDefault(w => !w.IsWhiteSpace);

                    length = match.Groups["error"].Length;
                }
                else
                {
                    word = segment.Words.FirstOrDefault(w => !w.IsWhiteSpace);
                    TextWord lastWord = segment.Words.Last(w => !w.IsWhiteSpace);
                    length = lastWord.Offset + lastWord.Length;
                }

                if (word != null)
                {
                    offset += word.Offset;
                }

                ErrorMarker marker = new ErrorMarker(offset, length, error.Description, error.IsWarning);
                _document.MarkerStrategy.AddMarker(marker);
            }
        }

        public void Dispose()
        {
        }
    }
}