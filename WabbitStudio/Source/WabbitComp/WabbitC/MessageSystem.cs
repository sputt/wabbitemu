using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WabbitC
{
	public sealed class MessageSystem
	{
		public enum ErrorCode
		{

		}
		public static MessageSystem Instance = new MessageSystem();
		public void ThrowNewWarning(ErrorCode error)
		{

		}

		public void ThrowNewError(ErrorCode error)
		{

		}

		public void ThrowNewWarning(string message)
		{
			
		}

		public void ThrowNewError(string message)
		{
			
		}
	}
}
