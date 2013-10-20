namespace Revsoft.Wabbitcode.Utils
{
	public class DocumentLocation
	{
		public string FileName { get; private set; }
		public int LineNumber { get; private set; }

		public DocumentLocation(string fileName, int lineNumber)
		{
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
			return string.Equals(FileName, other.FileName) && LineNumber == other.LineNumber;
		}

		public override int GetHashCode()
		{
			return ((FileName != null ? FileName.GetHashCode() : 0) * 397) ^ LineNumber;
		}

	    public override string ToString()
	    {
	        return FileName + ":" + LineNumber;
	    }
	}
}
