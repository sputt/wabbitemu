using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using Revsoft.Wabbitcode;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class ProjectFile
	{
		private IProject parent;
		public IProject Parent 
		{
			get { return parent; }
		}

		private string filePath;
		public string FileFullPath
		{
			get 
			{
				if (Path.IsPathRooted(filePath))
					return filePath;
				else
					return FileOperations.GetAbsolutePath(parent.ProjectDirectory, filePath);
			}
			set { filePath = value; }
		}

		public string FileRelativePath
		{
			get
			{
				if (Path.IsPathRooted(filePath))
					return FileOperations.GetRelativePath(parent.ProjectDirectory, filePath);
				else
					return filePath;
			}
			set { filePath = value; }
		}

		private string fileFoldings;
		public string FileFoldings
		{
			get { return fileFoldings; }
			set { fileFoldings = value; }
		}

		public ProjectFile(IProject project, string fullPath)
		{
			parent = project;
			filePath = fullPath;
		}

		public override string ToString()
		{
			return filePath;
		}

	}
}
