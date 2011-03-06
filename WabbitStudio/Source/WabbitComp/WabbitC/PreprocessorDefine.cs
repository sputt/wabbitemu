using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC
{
	public abstract class PreprocessorDefine
	{
		readonly Token name;
		public Token Name
		{
			get { return name; }
		}

		public PreprocessorDefine(Token name)
		{
			this.name = name;
		}

		public override string ToString()
		{
			return name.Text;
		}
	}

	public class MacroDefine : PreprocessorDefine
	{
		readonly List<Token> args;
		public List<Token> Args
		{
			get { return args; }
		}

		readonly List<Token> values;
		public List<Token> Values
		{
			get { return values; }
		}

		public MacroDefine(Token name, List<Token> args, List<Token> values)
			: base(name)
		{
			this.args = args;
			this.values = values;
		}
	}

	public class ReplacementDefine : PreprocessorDefine
	{
		readonly Token value;
		public Token Value
		{
			get { return value; }
		}

		public ReplacementDefine(Token name, Token value)
			: base(name)
		{
			this.value = value;
		}
	}
}
