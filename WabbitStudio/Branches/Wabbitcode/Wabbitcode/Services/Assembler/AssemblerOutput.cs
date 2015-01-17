using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Assembler
{
    public class AssemblerOutput
    {
        public string OutputText { get; private set; }

        private readonly List<BuildError> _parsedErrors;
        public IEnumerable<BuildError> ParsedErrors
        {
            get { return _parsedErrors; }
        }

        public bool Succeeded { get; private set; }

        public AssemblerOutput(string outputText, bool succeeded)
        {
            OutputText = outputText;
            Succeeded = succeeded;
            _parsedErrors = new List<BuildError>();
            ParseOutput();
        }

        private void ParseOutput()
        {
            _parsedErrors.Clear();
            string[] lines = OutputText.Split('\n');
            foreach (string line in lines)
            {
                Regex errorRegex = new Regex("(?<file>.+):(?<lineNum>\\d+):(?<type>.+):(?<description>.+)");
                Match match = errorRegex.Match(line);
                if (!match.Success)
                {
                    continue;
                }

                string filePath = match.Groups["file"].Value;
                if (filePath.IndexOfAny(Path.GetInvalidPathChars()) != -1)
                {
                    continue;
                }

                FilePath file = new FilePath(filePath);
                string lineNumString = match.Groups["lineNum"].Value;
                string type = match.Groups["type"].Value;
                string description = match.Groups["description"].Value;
                int lineNum = Convert.ToInt32(lineNumString);
                bool isWarning = type.Contains("warning");
                _parsedErrors.Add(new BuildError(file, lineNum, description, isWarning));
            }
        }
    }
}