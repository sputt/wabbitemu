using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Revsoft.Wabbitcode.Services.Project.Interface;
using Revsoft.Wabbitcode.Interface.Services;

namespace Revsoft.Wabbitcode.Services.Project
{
	class InternalProject : IProject
	{
		public string ProjectDirectory
		{
			get { return null; }
		}

		public string ProjectFile
		{
			get { return null; }
		}

		public string ProjectName { get; set; }

		public IProjectFolder MainFolder { get; private set; }

		public IList<string> IncludeDirs { get; private set; }

		public bool NeedsSave { get; set; }

		public IBuildSystem BuildSystem { get; private set; }

		public IAssemblerService AssemblerService { get; private set; }

		public IParserService ParserService { get; private set; }

		public IList<string> ProjectOutputs { get; private set; }

		public IList<string> LabelOutputs { get; private set; }

		public IList<string> ListOutputs { get; private set; }

		public InternalProject()
		{
			ParserService = new ParserService();
			BuildSystem = new BuildSystem(this, false);
		}

		public IProjectFolder AddFolder(string dirName, IProjectFolder parentFolder)
		{
			throw new NotImplementedException();
		}

		public IProjectFile AddFile(IProjectFolder parentFolder, string fullPath)
		{
			throw new NotImplementedException();
		}

		public bool ContainsFile(string fullPath)
		{
			throw new NotImplementedException();
		}

		public void DeleteFolder(IProjectFolder parentDir, IProjectFolder dir)
		{
			throw new NotImplementedException();
		}

		public void DeleteFile(IProjectFile file)
		{
			throw new NotImplementedException();
		}

		public void DeleteFile(string fullPath)
		{
			throw new NotImplementedException();
		}

		public IProjectFile FindFile(string fullPath)
		{
			throw new NotImplementedException();
		}

		public IList<IProjectFile> GetProjectFiles()
		{
			throw new NotImplementedException();
		}

		public Project.AssemblerOutputType GetOutputType()
		{
			throw new NotImplementedException();
		}

		public void SaveProjectFile(System.IO.Stream stream)
		{
			throw new InvalidOperationException();
		}
	}
}
