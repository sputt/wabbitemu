using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Revsoft.Wabbitcode.Services.Project
{
	public class ProjectFolder
	{
		private string name;
		public string Name
		{
			get { return name; }
		}

		private IProject parent;
		public IProject Parent
		{
			get { return parent; }
		}

		private List<ProjectFolder> folders;
		public List<ProjectFolder> Folders
		{
			get { return folders; }
		}

		private List<ProjectFile> files;
		public List<ProjectFile> Files
		{
			get { return files; }
		}

		public ProjectFolder(IProject parent, string folderName)
		{
			this.parent = parent;
			this.name = folderName;
			files = new List<ProjectFile>();
			folders = new List<ProjectFolder>();
		}

		public override string ToString()
		{
			return name;
		}

		/// <summary>
		/// Searches for a folder in subfolders.
		/// </summary>
		/// <param name="folder"></param>
		/// <returns>Returns folder found, otherwise null</returns>
		internal ProjectFolder FindFolder(string folder)
		{
			foreach (ProjectFolder subFolder in folders)
				if (subFolder.name == folder)
					return subFolder;
			return null;
		}

		/// <summary>
		/// Returns if the filename is contained in the list of files
		/// </summary>
		/// <param name="file">The file name to search for, not full path</param>
		/// <returns>File found otherwise null</returns>
		internal ProjectFile FindFile(string file)
		{
			foreach (ProjectFile subFile in files)
				if (Path.GetFileName(subFile.FileFullPath) == file)
					return subFile;
			return null;
		}
	}
}
