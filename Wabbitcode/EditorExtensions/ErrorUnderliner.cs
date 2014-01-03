using System;
using System.Linq;
using System.Text.RegularExpressions;
using Revsoft.TextEditor.Document;
using Revsoft.Wabbitcode.EditorExtensions.Markers;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode.EditorExtensions
{
    internal class ErrorUnderliner : IDisposable
    {
        private readonly WabbitcodeTextEditor _wabbitcodeTextEditor;
        private readonly IDocument _document;

        public ErrorUnderliner(WabbitcodeTextEditor wabbitcodeTextEditor)
        {
            _wabbitcodeTextEditor = wabbitcodeTextEditor;
            _document = _wabbitcodeTextEditor.Document;

            IBackgroundAssemblerService backgroundAssemblerService = ServiceFactory.Instance.GetServiceInstance<IBackgroundAssemblerService>();
            backgroundAssemblerService.BackgroundAssemblerComplete += BackgroundAssemblerService_BackgroundAssemblerComplete;
        }

        private void BackgroundAssemblerService_BackgroundAssemblerComplete(object sender, AssemblyFinishEventArgs e)
        {
            _document.MarkerStrategy.RemoveAll(m => m is ErrorMarker);

            foreach (BuildError error in e.Output.ParsedErrors.Where(
                error => FileOperations.CompareFilePath(error.File, _wabbitcodeTextEditor.FileName)))
            {
                int offset;
                int endOffset;
                var segment =  _document.GetLineSegment(error.LineNumber - 1);
                Match match = Regex.Match(error.Description, "'(?<error>.*?)'");
                string line = string.Empty;
                BuildError errorCopy = error;
                _wabbitcodeTextEditor.Invoke(() => line = _wabbitcodeTextEditor.GetLineText(errorCopy.LineNumber - 1));
                if (match.Success)
                {
                    offset = segment.Offset + line.IndexOf(match.Groups["error"].Value, StringComparison.Ordinal);
                    endOffset = offset + match.Groups["error"].Length;
                }
                else
                {
                    offset = segment.Offset + segment.Words.First(w => !w.IsWhiteSpace).Offset;
                    TextWord lastWord = segment.Words.Last(w => !w.IsWhiteSpace);
                    endOffset = lastWord.Offset + lastWord.Length;
                }

                ErrorMarker marker = new ErrorMarker(offset, endOffset - offset, error.Description, error.IsWarning);
                _document.MarkerStrategy.AddMarker(marker);
            }
        }

        public void Dispose()
        {
        }
    }
}