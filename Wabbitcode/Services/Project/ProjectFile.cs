using System.IO;
using Revsoft.Wabbitcode.Services.Project.Interface;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class ProjectFile : IProjectFile
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

		public string FileFoldings { get; set; }

		public IProjectFolder Folder { get; set; }


		public ProjectFile(IProject project, string fullPath)
		{
			parent = project;
			filePath = fullPath;
		}

		public override string ToString()
		{
			return filePath;
		}

		public override int GetHashCode()
		{
			return filePath.GetHashCode();
		}

		public override bool Equals(object obj)
		{
			IProjectFile file = obj as IProjectFile;
			if (file == null)
			{
				return base.Equals(obj);
			}
			return FileFullPath.Equals(file.FileFullPath);
		}


		public void Remove()
		{
			Folder.Files.Remove(this);
			Folder = null;
		}
	}
}
