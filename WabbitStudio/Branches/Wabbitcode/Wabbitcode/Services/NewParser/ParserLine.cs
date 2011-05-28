using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Services.Parser;
using System.Text.RegularExpressions;

namespace Revsoft.Wabbitcode.Services.NewParser
{
    public class ParserLine : List<IParserData>
    {
        ParserInformation parser;
        public ParserLine(ParserInformation parser)
        {
            this.parser = parser;
        }

        static Regex labelRegex = new Regex(@"(^(?<name>[A-Za-z0-9_]+):*)(?!\()\b", RegexOptions.Compiled);
		static Regex defineRegex = new Regex(@"^\s*#define\s+(?<name>[A-Za-z_.]+)\(*(?<args>(?<=(?<open>\()).*(?=(?<close-open>\))))\)*\s+(?<contents>(.+))", RegexOptions.Compiled);
        public static ParserLine ParseLine(string line, ParserInformation parser)
        {
            var lineData = new ParserLine(parser);
            if (labelRegex.IsMatch(line))
			{
                    var match = labelRegex.Match(line);
                    var capture = match.Captures[0];
                    var label = new Label(new DocLocation(0, capture.Index), capture.Value, "", parser);
                    lineData.Add(label);
			}
			else if (defineRegex.IsMatch(line))
			{
				var match = defineRegex.Match(line);
				var args = match.Groups["args"].Value.Split(',').ToList();
				string name = match.Groups["name"].Value;
				string contents = match.Groups["contents"].Value;
				var def = new Define(new DocLocation(0, match.Groups["contents"].Index), name, contents, "", parser, 0x4000);//ParserService.EvaluateContents(contents));
				lineData.Add(def);
			}

            return lineData;
        }
    }
}
