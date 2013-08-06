using System.Linq;

namespace Revsoft.Wabbitcode.Services.Project
{
	using System;
	using System.Collections.Generic;
	using System.IO;

	public class ProjectFolder
	{
		private List<ProjectFile> files;
		private List<ProjectFolder> folders;
		private string name;
		private IProject parent;
		private ProjectFolder parentFolder;

		public ProjectFolder(IProject parent, string folderName)
		{
			this.parent = parent;
			name = folderName;
			files = new List<ProjectFile>();
			folders = new List<ProjectFolder>();
		}

		public List<ProjectFile> Files
		{
			get
			{
				return files;
			}
		}

		public List<ProjectFolder> Folders
		{
			get
			{
				return folders;
			}
		}

		public string Name
		{
			get
			{
				return name;
			}
			set
			{
				name = value;
			}
		}

		public IProject Parent
		{
			get
			{
				return parent;
			}
		}

		private ProjectFolder ParentFolder
		{
			get
			{
				return parentFolder;
			}
			set
			{
				parentFolder = value;
			}
		}

		public override string ToString()
		{
			return name;
		}

		internal void AddFile(ProjectFile file)
		{
			files.Add(file);
			file.Folder = this;
		}

		internal void AddFolder(ProjectFolder subFolder)
		{
			folders.Add(subFolder);
			subFolder.ParentFolder = this;
		}

		/// <summary>
		/// Returns if the filename is contained in the list of files
		/// </summary>
		/// <param name="file">The file name to search for, not full path</param>
		/// <returns>File found otherwise null</returns>
		internal ProjectFile FindFile(string file)
		{
			return files.FirstOrDefault(subFile => string.Equals(Path.GetFileName(subFile.FileFullPath), file, StringComparison.OrdinalIgnoreCase));
		}

		/// <summary>
		/// Searches for a folder in subfolders.
		/// </summary>
		/// <param name="folder"></param>
		/// <returns>Returns folder found, otherwise null</returns>
		internal ProjectFolder FindFolder(string folder)
		{
			return folders.FirstOrDefault(subFolder => subFolder.name == folder);
		}

		// TODO: fix
		internal void Remove()
		{
			parentFolder.Folders.Remove(this);
			parentFolder = null;
		}
	}
}