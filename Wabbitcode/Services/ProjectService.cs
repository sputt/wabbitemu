﻿using System.Threading.Tasks;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.Services.Project;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Revsoft.Wabbitcode.Services
{
	[ServiceDependency(typeof(IAssemblerService))]
	[ServiceDependency(typeof(IParserService))]
	[ServiceDependency(typeof(ISymbolService))]
	public class ProjectService : IProjectService
	{
		private readonly List<ParserInformation> _parseInfo = new List<ParserInformation>();
		private readonly IParserService _parserService;
		private readonly IAssemblerService _assemblerService;
		private readonly ISymbolService _symbolService;

		private IList<ParserInformation> ParserInfomInformation
		{
			get
			{
				return _parseInfo;
			}
		}

		public WabbitcodeProject Project { get; private set; }

		public ProjectService(IAssemblerService assemblerService, IParserService parserService, ISymbolService symbolService)
		{
			_assemblerService = assemblerService;
			_parserService = parserService;
			_symbolService = symbolService;
		}

		public bool OpenProject(string fileName)
		{
			Project = new WabbitcodeProject(fileName, _assemblerService);
			Project.OpenProject(fileName);
			_symbolService.ProjectDirectory = Project.ProjectDirectory;

			_parseInfo.Clear();
			Task.Factory.StartNew(() => ParseFiles(Project.MainFolder));

			return true;
		}

		public ProjectFile AddFile(ProjectFolder parent, string fullPath)
		{
			ProjectFile file = new ProjectFile(parent, fullPath, Project.ProjectDirectory);
			parent.AddFile(file);
			return file;
		}

		public ProjectFolder AddFolder(string dirName, ProjectFolder parentDir)
		{
			ProjectFolder folder = new ProjectFolder(parentDir, dirName);
			parentDir.AddFolder(folder);
			return folder;
		}

		public void CloseProject()
		{
			if (Project.IsInternal)
			{
				Project = null;
				return;
			}

			CreateInternalProject();
		}

		public bool ContainsFile(string file)
		{
			return file != null && Project.ContainsFile(file);
		}

		public void CreateInternalProject()
		{
			Project = new WabbitcodeProject(_assemblerService)
			{
				IsInternal = true
			};
		}

		public void CreateNewProject(string projectFile, string projectName)
		{
			Project = new WabbitcodeProject(_assemblerService);
			Project.CreateNewProject(projectFile, projectName);
		}

		public void DeleteFile(string fullPath)
		{
			ProjectFile file = Project.FindFile(fullPath);
			DeleteFile(file.Folder, file);
		}

		public void DeleteFile(ProjectFolder parentDir, ProjectFile file)
		{
			RemoveParseData(file.FileFullPath);
			parentDir.DeleteFile(file);
		}

		public void DeleteFolder(ProjectFolder parentDir, ProjectFolder dir)
		{
			parentDir.DeleteFolder(dir);
		}

		public void RemoveParseData(string fullPath)
		{
			ParserInformation replaceMe = GetParseInfo(fullPath);
			if (replaceMe != null)
			{
				ParserInfomInformation.Remove(replaceMe);
			}
		}

		public ParserInformation GetParseInfo(string file)
		{
			lock (_parseInfo)
			{
				foreach (var info in _parseInfo.Where(info => string.Equals(info.SourceFile, file)))
				{
					return info;
				}
			}

			return null;
		}

		public void SaveProject()
		{
			Project.SaveProject();
		}

		private void ParseFiles(ProjectFolder folder)
		{
			foreach (ProjectFolder subFolder in folder.Folders)
			{
				ParseFiles(subFolder);
			}

			ProjectFile[] filesToParse = new ProjectFile[folder.Files.Count];
			folder.Files.CopyTo(filesToParse, 0);
			foreach (ProjectFile file in filesToParse)
			{
				_parserService.ParseFile(0, file.FileFullPath);
			}
		}

		public IEnumerable<List<Reference>> FindAllReferences(string refString)
		{
			var refsList = new List<List<Reference>>();
			var files = Project.GetProjectFiles();
			refsList.AddRange(files.Select(file =>
			{
				string filePath = Path.Combine(Project.ProjectDirectory, file.FileFullPath);
				return _parserService.FindAllReferencesInFile(filePath, refString);
			}).Where(refs => refs.Count > 0));
			return refsList;
		}


		#region IService
		public void DestroyService()
		{
			
		}

		public void InitService(params object[] objects)
		{
			
		}
		#endregion
	}
}