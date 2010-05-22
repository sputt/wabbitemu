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

namespace Revsoft.Wabbitcode.Services
{
	public static class ProjectService
	{
		private static ProjectClass project;
		public static ProjectClass Project
		{
			get { return project; }
			set { project = value; }
		}

		public static string ProjectDirectory
		{
			get { return project.projectLoc; }
		}

		private static List<ParserInformation> parseInfo = new List<ParserInformation>();
		public static IList<ParserInformation> ParseInfo
		{
			get { return parseInfo; }
		}

		public static string ProjectFile
		{
			get { return project.wcodeProjectFile; }
		}

		public static string ProjectName
		{
			get { return project.projectName; }
			set { project.projectName = value; }
		}

		public static bool IsInternal
		{
			get
			{
				if (project == null)
					return true;
				return project.isInternal; }
		}

		public static string[] IncludeDirs {
			get { return project.getIncludeDirs(); }
			set { project.setIncludeDirs(value); }
		}

		static FileSystemWatcher projectWatcher;
		public static FileSystemWatcher ProjectWatcher {
			get { return projectWatcher; }
		}

		public static void OpenProject(string fileName)
		{
			OpenProject(fileName, false);
		}

		public static void OpenProject(string fileName, bool closeFiles)
		{
			if (closeFiles)
				foreach (Form mdiChild in DockingService.Documents)
					mdiChild.Close();
			project = new ProjectClass(fileName);
			DockingService.DirectoryViewer.buildDirectoryTree(Directory.GetFiles(ProjectDirectory));
			InitWatcher(project.projectLoc);
			DockingService.ShowDockPanel(DockingService.ProjectViewer);
			DockingService.ProjectViewer.buildProjTree(ProjectFile);
			DockingService.MainForm.UpdateProjectMenu(true);

			ThreadStart threadStart = new ThreadStart(GetIncludeDirectories);
			Thread thread = new Thread(threadStart);
			thread.Start();
		}

		private static void GetIncludeDirectories()
		{
			string[] mainDirs = Directory.GetDirectories(ProjectService.ProjectDirectory);
			Settings.Default.includeDir += getProjectIncludeDirectories(mainDirs);
			ArrayList files = ProjectService.Project.getAllProjectFiles();
			ProjectService.Project.getAllProjectLabels(files);
		}

		private static string getProjectIncludeDirectories(IEnumerable<string> directories)
		{
			string includeDir = "";
			foreach (String directory in directories)
			{
				getProjectIncludeDirectories(Directory.GetDirectories(directory));
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
			throw new NotImplementedException();
		}

		delegate void FileChangedDelegate(newEditor doc, string fileName);
		static void projectWatcher_Changed(object sender, FileSystemEventArgs e)
		{
			switch (e.ChangeType)
			{
				case WatcherChangeTypes.Changed:
					if (!string.IsNullOrEmpty(Path.GetExtension(e.FullPath)))
					{
						foreach (newEditor doc in DockingService.Documents)
						{
							if (doc.editorBox.FileName.Equals(e.FullPath, StringComparison.OrdinalIgnoreCase))
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
			project = new ProjectClass("");
		}

		internal static void CreateNewProject(string projectFile)
		{
			project = new ProjectClass(projectFile);
		}

		internal static void DeleteFolder(string parentDir, string dir)
		{
			project.deleteFolder(parentDir, dir);
		}

		internal static void SaveProject()
		{
			project.saveProject();
		}

		internal static void AddDir(string dir, string parentDir, bool addchildren)
		{
			project.addDir(dir, parentDir, addchildren);
		}

		internal static void AddFile(string file, string dir, string fullPath)
		{
			project.addFile(file, dir, fullPath);
		}

		internal static void CloseProject()
		{
			project.CloseProject();
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
	}
}
