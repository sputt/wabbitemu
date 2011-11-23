using System;
using Revsoft.Wabbitcode.Interface;
using System.IO;

namespace Revsoft.Wabbitcode.Utilities
{
	public class FilePath : IFilePath
	{
		public string Path
		{
			get;
			set;
		}

		public FilePath(string filePath)
		{
			Path = filePath;
		}

		public override bool Equals(object obj)
		{
			if (!(obj is FilePath || obj is string))
				return base.Equals(obj);
			return string.Equals(obj.ToString(), this.ToString(), StringComparison.OrdinalIgnoreCase);
		}

		public override int GetHashCode()
		{
			return this.ToString().GetHashCode();
		}

		public override string ToString()
		{
			return Path;
		}

		public static implicit operator string(FilePath path)
		{
			return path.Path;
		}

		#region Static Methods

		public static FilePath ChangeExtention(FilePath path, string ext)
		{
			return new FilePath(System.IO.Path.ChangeExtension(path, ext));
		}

		#endregion
	}
}
