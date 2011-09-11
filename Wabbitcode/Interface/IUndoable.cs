using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Interface
{
	public interface IUndoable
	{
		void Undo();

		void Redo();
	}
}
