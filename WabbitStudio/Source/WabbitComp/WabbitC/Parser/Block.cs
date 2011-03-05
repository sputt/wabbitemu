using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC.Parser
{
	public class Block
	{
		List<TypeClass> types = new List<TypeClass>();
		List<Declaration> declarations = new List<Declaration>();
		List<Statement> statements = new List<Statement>();
	}
}
