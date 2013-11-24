using System;
using System.Collections.Generic;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Assembler
{
	public class AssemblerOutput
	{
		public AssemblerOutput(string outputText, bool succeeded)
		{
			OutputText = outputText;
			Succeeded = succeeded;
			ParsedErrors = new List<BuildError>();
			ParseOutput();
		}

		public string OutputText
		{
			get;
			private set;
		}

		public List<BuildError> ParsedErrors
		{
			get;
			private set;
		}

		private bool Succeeded
		{
			get;
			set;
		}

		private void ParseOutput()
		{
			ParsedErrors.Clear();
			string[] lines = OutputText.Split('\n');
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
						ParsedErrors[ParsedErrors.Count - 1].Description += line;
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
						ParsedErrors.Add(new BuildError(file, lineNumber, description, false));
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
					ParsedErrors[ParsedErrors.Count - 1].Description += line;
				}
				else
				{
					// file = Path.Combine(startDir, line.Substring(0, firstColon));
					file = line.Substring(0, firstColon);
					lineNum = line.Substring(firstColon + 1, secondColon - firstColon - 1);
					description = line.Substring(thirdColon + 2, line.Length - thirdColon - 2);
					ParsedErrors.Add(new BuildError(file, Convert.ToInt32(lineNum), description, true));
				}
			}
		}
	}
}