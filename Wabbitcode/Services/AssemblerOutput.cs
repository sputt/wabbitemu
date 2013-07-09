namespace Revsoft.Wabbitcode.Services.Assembler
{
    using Revsoft.Wabbitcode.Classes;
    using System;
    using System.Collections.Generic;

    public class AssemblerOutput
    {
        public AssemblerOutput(string outputText, bool succeeded)
        {
            this.OutputText = outputText;
            this.Succeeded = succeeded;
            this.ParsedErrors = new List<Errors>();
            this.ParseOutput();
        }

        public string OutputText
        {
            get;
            private set;
        }

        public List<Errors> ParsedErrors
        {
            get;
            private set;
        }

        public bool Succeeded
        {
            get;
            private set;
        }

        private void ParseOutput()
        {
            this.ParsedErrors.Clear();
            string[] lines = this.OutputText.Split('\n');
            foreach (string line in lines)
            {
                int thirdColon, secondColon, firstColon;
                string file, lineNum, description;
                if (line.Contains("error"))
                {
                    firstColon = line.IndexOf(':', 3);
                    secondColon = line.IndexOf(':', firstColon + 1);
                    thirdColon = line.IndexOf(':', secondColon + 1);
                    if (firstColon < 0 || secondColon < 0 || thirdColon < 0)
                    {
                        this.ParsedErrors[this.ParsedErrors.Count - 1].Description += line;
                    }
                    else
                    {
                        // file = Path.Combine(startDir, line.Substring(0, firstColon));
                        file = line.Substring(0, firstColon);
                        lineNum = line.Substring(firstColon + 1, secondColon - firstColon - 1);
                        int lineNumber;
                        if (!int.TryParse(lineNum, out lineNumber))
                        {
                            lineNumber = -1;
                        }

                        description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
                        this.ParsedErrors.Add(new Errors(file, lineNumber, description, false));
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
                    this.ParsedErrors[this.ParsedErrors.Count - 1].Description += line;
                }
                else
                {
                    // file = Path.Combine(startDir, line.Substring(0, firstColon));
                    file = line.Substring(0, firstColon);
                    lineNum = line.Substring(firstColon + 1, secondColon - firstColon - 1);
                    description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
                    this.ParsedErrors.Add(new Errors(file, Convert.ToInt32(lineNum), description, true));
                }
            }
        }
    }
}