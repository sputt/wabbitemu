using System.Collections.Generic;
using System.IO;
using Revsoft.Wabbitcode.Services.Project.Interface;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class ProjectFolder : IProjectFolder
	{
		public string Name { get; set; }

		private IProject parent;
		public IProject Parent
		{
			get { return parent; }
		}

		private IList<IProjectFolder> folders;
		public IList<IProjectFolder> Folders
		{
			get { return folders; }
		}

		private IList<IProjectFile> files;
		public IList<IProjectFile> Files
		{
			get { return files; }
		}

		public IProjectFolder ParentFolder { get; set; }

		public ProjectFolder(IProject parent, string folderName)
		{
			this.parent = parent;
			this.Name = folderName;
			files = new List<IProjectFile>();
			folders = new List<IProjectFolder>();
		}

		public override string ToString()
		{
			return Name;
		}

		public override bool Equals(object obj)
		{
			IProjectFolder folder = obj as IProjectFolder;
			if (folder == null)
			{
				return base.Equals(obj);
			}
			return Name.Equals(folder.Name);
		}

		public override int GetHashCode()
		{
			return Name.GetHashCode();
		}

		/// <summary>
		/// Searches for a folder in subfolders.
		/// </summary>
		/// <param name="folder"></param>
		/// <returns>Returns folder found, otherwise null</returns>
		public IProjectFolder FindFolder(string folder)
		{
			foreach (IProjectFolder subFolder in folders)
				if (subFolder.Name == folder)
					return subFolder;
			return null;
		}

		/// <summary>
		/// Returns if the filename is contained in the list of files
		/// </summary>
		/// <param name="file">The file name to search for, not full path</param>
		/// <returns>File found otherwise null</returns>
		public IProjectFile FindFile(string file)
		{
			foreach (IProjectFile subFile in files)
				if (Path.GetFileName(subFile.FileFullPath).ToLower() == file.ToLower())
					return subFile;
			return null;
		}

		public void AddFile(IProjectFile file)
		{
			files.Add(file);
			file.Folder = this;
			parent.NeedsSave = true;
		}

		public void AddFolder(IProjectFolder subFolder)
		{
			folders.Add(subFolder);
			subFolder.ParentFolder = this;
			parent.NeedsSave = true;
		}

		public void Remove()
		{
			ParentFolder.Folders.Remove(this);
			ParentFolder = null;
			parent.NeedsSave = true;
		}
	}
}
