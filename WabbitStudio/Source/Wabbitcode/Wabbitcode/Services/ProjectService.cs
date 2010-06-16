using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Docking_Windows;
using System.Collections;
using Revsoft.Wabbitcode.Properties;
using System.Threading;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode.Services
{
	public static class ProjectService
	{
		private static ProjectFile currentFile;
		public static ProjectFile CurrentFile
		{
			get { return currentFile; }
			set { currentFile = value; }
		}

		private static ProjectClass project;
		public static ProjectClass Project
		{
			get { return project; }
			set { project = value; }
		}

		public static string ProjectDirectory
		{
			get { return project.ProjectDirectory; }
		}

		private static List<ParserInformation> parseInfo = new List<ParserInformation>();
		public static IList<ParserInformation> ParseInfo
		{
			get { return parseInfo; }
		}

		public static string ProjectFile
		{
			get { return project.ProjectFile; }
		}

		public static string ProjectName
		{
			get { return project.ProjectName; }
			set { project.ProjectName = value; }
		}

		private static bool isInternal = true;
		public static bool IsInternal
		{
			get { return isInternal; }
			set { isInternal = value; }
		}

		public static List<string> IncludeDirs {
			get { return project.IncludeDir; }
			set { project.IncludeDir = value; }
		}

		public static ProjectFolder MainFolder
		{
			get
			{
				if (project == null)
					return null;
				return project.MainFolder;
			}
			set { project.MainFolder = value; }
		}

		static FileSystemWatcher projectWatcher;
		public static FileSystemWatcher ProjectWatcher {
			get { return projectWatcher; }
		}

		public static void OpenProject(string fileName)
		{
			OpenProject(fileName, false);
			DockingService.MainForm.UpdateProjectMenu(true);
		}

		public static void OpenProject(string fileName, bool closeFiles)
		{
			if (closeFiles)
				foreach (Form mdiChild in DockingService.Documents)
					mdiChild.Close();
			project = new ProjectClass(fileName);
			project.OpenProject(fileName);
			DockingService.DirectoryViewer.buildDirectoryTree(Directory.GetFiles(ProjectDirectory));
			InitWatcher(project.ProjectDirectory);
			DockingService.ShowDockPanel(DockingService.ProjectViewer);
			DockingService.ProjectViewer.BuildProjTree();
			DockingService.MainForm.UpdateProjectMenu(true);

			//ThreadStart threadStart = new ThreadStart(GetIncludeDirectories);
			Thread thread = new Thread(ParseFiles);
			thread.Priority = ThreadPriority.BelowNormal;
			thread.Start();
		}

		private static void ParseFiles()
		{
			ParseFiles(MainFolder);
		}

		private static void ParseFiles(ProjectFolder folder)
		{
			foreach (ProjectFolder subFolder in folder.Folders)
				ParseFiles(subFolder);
			foreach (ProjectFile file in folder.Files)
				ParserService.ParseFile(file.FileFullPath);
		}

		private static string GetProjectIncludeDirectories(IEnumerable<string> directories)
		{
			string includeDir = "";
			foreach (string directory in directories)
			{
				GetProjectIncludeDirectories(Directory.GetDirectories(directory));
				includeDir += directory + "\n";
			}
			return includeDir;
		}

		private static void InitWatcher(string location)
		{
			projectWatcher = new FileSystemWatcher(location);
			projectWatcher.Changed += new FileSystemEventHandler(projectWatcher_Changed);
			projectWatcher.Deleted += new FileSystemEventHandler(projectWatcher_Deleted);
			projectWatcher.Renamed += new RenamedEventHandler(projectWatcher_Renamed);
			projectWatcher.Created += new FileSystemEventHandler(projectWatcher_Created);
			projectWatcher.EnableRaisingEvents = true;
			projectWatcher.IncludeSubdirectories = true;
			projectWatcher.Path = location;
		}

		static void projectWatcher_Created(object sender, FileSystemEventArgs e)
		{

		}

		static void projectWatcher_Renamed(object sender, RenamedEventArgs e)
		{
			if (e.OldFullPath == ProjectDirectory)
			{
				if (MessageBox.Show("Project Folder was renamed, would you like to rename the project?", "Rename project",
					MessageBoxButtons.YesNo, MessageBoxIcon.Information) == DialogResult.Yes)
					ProjectName = Path.GetFileNameWithoutExtension(e.FullPath);
				return;
			}
		}

		static void projectWatcher_Deleted(object sender, FileSystemEventArgs e)
		{
			//throw new NotImplementedException();
		}

		delegate void FileChangedDelegate(newEditor doc, string fileName);
		static void projectWatcher_Changed(object sender, FileSystemEventArgs e)
		{
			switch (e.ChangeType)
			{
				case WatcherChangeTypes.Changed:
					if (!DocumentService.InternalSave && !string.IsNullOrEmpty(Path.GetExtension(e.FullPath)))
					{
						foreach (newEditor doc in DockingService.Documents)
						{
							if (doc.FileName.Equals(e.FullPath, StringComparison.OrdinalIgnoreCase))
							{
								FileChangedDelegate fileChanged = UpdateFileChanged;
								DockingService.MainForm.Invoke(fileChanged, new object[] { doc, e.FullPath });
								break;
							}
						}
					}
					break;
			}
		}

		private static void UpdateFileChanged(newEditor doc, string fileName)
		{
			projectWatcher.EnableRaisingEvents = false;
			DialogResult result = MessageBox.Show(fileName + " modified outside the editor.\nLoad changes?", "File modified", MessageBoxButtons.YesNo);
			if (result == System.Windows.Forms.DialogResult.Yes)
				DocumentService.OpenDocument(doc, fileName);
			projectWatcher.EnableRaisingEvents = true;
		}

		internal static void CreateInternalProject()
		{
			project = new ProjectClass();
			DockingService.MainForm.UpdateProjectMenu(false);
		}

		internal static void CreateNewProject(string projectFile, string projectName)
		{
			project = new ProjectClass();
			project.CreateNewProject(projectFile, projectName);
			DockingService.ShowDockPanel(DockingService.ProjectViewer);
		}

		internal static void DeleteFolder(string parentDir, string dir)
		{
			//project.deleteFolder(parentDir, dir);
		}

		internal static void SaveProject()
		{
			project.BuildXMLFile();
			DockingService.ProjectViewer.BuildProjTree();
		}

		internal static ProjectFolder AddDir(string dir, ProjectFolder parentDir)
		{
			ProjectFolder folder = new ProjectFolder(project, dir);
			parentDir.Folders.Add(folder);
			return folder;
		}

		internal static void AddFile(ProjectFolder parent, string fullPath)
		{
			ProjectFile file = new ProjectFile(project, fullPath);
			parent.Files.Add(file);
			//project.addFile(file, dir, fullPath);
		}

		internal static void CloseProject()
		{
			//project.CloseProject();
		}

		internal static ParserInformation GetParseInfo()
		{
			return GetParseInfo(DocumentService.ActiveFileName);
		}

		internal static ParserInformation GetParseInfo(string file)
		{
			foreach (ParserInformation info in parseInfo)
				if (info.SourceFile.ToLower() == file.ToLower())
					return info;
			return null;
		}



		internal static System.Xml.XmlNodeList GetBuildConfigs()
		{
			throw new NotImplementedException();
		}

		internal static bool ContainsFile(string file)
		{
			return RecurseSearchFolders(MainFolder, Path.GetFileName(file));
		}

		private static ProjectFile fileFound;
		private static bool RecurseSearchFolders(ProjectFolder folder, string file)
		{
			fileFound = folder.FindFile(file);
				if (fileFound != null)
					return true;
			foreach (ProjectFolder subFolder in folder.Folders)
				RecurseSearchFolders(subFolder, file);
			return false;
		}

		internal static ProjectFile FindFile(string file)
		{
			if (fileFound.FileFullPath == file)
				return fileFound;
			if (ContainsFile(file))
				return fileFound;
			return null;
		}

		internal static void ActiveFileChanged()
		{
			if (ContainsFile(DocumentService.ActiveFileName))
				currentFile = fileFound;
		}

		public static IList<BuildConfig> BuildConfigs
		{
			get { return project.BuildSystem.BuildConfigs; }
		}

		public static int CurrentConfigIndex
		{
			get { return project.BuildSystem.CurrentConfigIndex; }
		}

		public static BuildConfig CurrentBuildConfig
		{
			get { return project.BuildSystem.CurrentConfig; }
		}
	}
}
