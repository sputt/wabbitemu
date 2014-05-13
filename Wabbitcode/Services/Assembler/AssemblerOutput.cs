using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Assembler
{
    public class AssemblerOutput
    {
        private readonly List<BuildError> _parsedErrors;

        public AssemblerOutput(string outputText, bool succeeded)
        {
            OutputText = outputText;
            Succeeded = succeeded;
            _parsedErrors = new List<BuildError>();
            ParseOutput();
        }

        public string OutputText { get; private set; }

        public IEnumerable<BuildError> ParsedErrors
        {
            get { return _parsedErrors; }
        }

        public bool Succeeded { get; private set; }

        private void ParseOutput()
        {
            _parsedErrors.Clear();
            string[] lines = OutputText.Split('\n');
            foreach (string line in lines)
            {
                int thirdColon, secondColon, firstColon;
                FilePath file;
                string lineNum, description;
                if (line.Contains("error"))
                {
                    firstColon = line.IndexOf(':', 3);
                    secondColon = line.IndexOf(':', firstColon + 1);
                    thirdColon = line.IndexOf(':', secondColon + 1);
                    if (firstColon < 0 || secondColon < 0 || thirdColon < 0)
                    {
                        _parsedErrors[_parsedErrors.Count - 1].Description += line;
                    }
                    else
                    {
                        // file = Path.Combine(startDir, line.Substring(0, firstColon));
                        file = new FilePath(line.Substring(0, firstColon));
                        lineNum = line.Substring(firstColon + 1, secondColon - firstColon - 1);
                        int lineNumber;
                        if (!int.TryParse(lineNum, out lineNumber))
                        {
                            lineNumber = -1;
                        }

                        description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
                        _parsedErrors.Add(new BuildError(file, lineNumber, description, false));
                    }
                }

                if (!line.Contains("warning"))
                {
                    continue;
                }
                firstColon = line.IndexOf(':', 3);
                secondColon = line.IndexOf(':', firstColon + 1);
                thirdColon = line.IndexOf(':', secondColon + 1);
                if (firstColon < 0 || secondColon < 0 || thirdColon < 0)
                {
                    _parsedErrors[_parsedErrors.Count - 1].Description += line;
                }
                else
                {
                    file = new FilePath(line.Substring(0, firstColon));
                    lineNum = line.Substring(firstColon + 1, secondColon - firstColon - 1);
                    description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
                    _parsedErrors.Add(new BuildError(file, Convert.ToInt32(lineNum), description, true));
                }
            }
        }
    }
}