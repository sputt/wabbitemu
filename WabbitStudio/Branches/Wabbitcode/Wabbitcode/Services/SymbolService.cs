using Revsoft.Wabbitcode.Annotations;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Symbols;

namespace Revsoft.Wabbitcode.Services
{
    [UsedImplicitly]
    public class SymbolService : ISymbolService
	{
		private readonly SymbolTable _symbolTable = new SymbolTable();
		private readonly ListTable _listTable = new ListTable();
        private readonly IProjectService _projectService;
        private string _projectDirectory;

		public SymbolTable SymbolTable
		{
			get { return _symbolTable; }
		}

		public ListTable ListTable
		{
			get { return _listTable; }
		}

		public void ParseSymbolFile(string symbolContents)
		{
			_symbolTable.ParseSymFile(symbolContents);
		}

		public void ParseListFile(string labelContents)
		{
			_listTable.ParseListFile(labelContents);
		}

		public SymbolService(IProjectService projectService)
		{
		    _projectService = projectService;
            _projectService.ProjectOpened += (sender, args) => _projectDirectory = _projectService.Project.ProjectDirectory;
		}
	}
}
