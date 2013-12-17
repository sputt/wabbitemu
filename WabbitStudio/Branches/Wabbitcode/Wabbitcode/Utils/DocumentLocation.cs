using System;
using Revsoft.Wabbitcode.Extensions;

namespace Revsoft.Wabbitcode.Utils
{
	public class DocumentLocation
	{
		public string FileName { get; private set; }
		public int LineNumber { get; private set; }

		public DocumentLocation(string fileName, int lineNumber)
		{
		    if (string.IsNullOrEmpty(fileName))
		    {
		        throw new ArgumentNullException("fileName");
		    }

		    if (lineNumber < 0)
		    {
		        throw new ArgumentOutOfRangeException("lineNumber");
		    }

			FileName = fileName;
			LineNumber = lineNumber;
		}

		public override bool Equals(object obj)
		{
			if (ReferenceEquals(null, obj))
			{
				return false;
			}

			if (ReferenceEquals(this, obj))
			{
				return true;
			}

			return obj.GetType() == GetType() && Equals((DocumentLocation) obj);
		}

		private bool Equals(DocumentLocation other)
		{
			return FileOperations.CompareFilePath(FileName, other.FileName) && LineNumber == other.LineNumber;
		}

		public override int GetHashCode()
		{
			return ((FileName.ToLower().GetHashCode()) * 397) ^ LineNumber;
		}

	    public override string ToString()
	    {
	        return FileName + ":" + LineNumber;
	    }
	}
}
