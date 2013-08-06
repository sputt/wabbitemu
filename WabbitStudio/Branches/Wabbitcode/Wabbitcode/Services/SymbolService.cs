using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Symbols;

namespace Revsoft.Wabbitcode.Services
{
	public class SymbolService : ISymbolService
	{
		private readonly SymbolTable _symbolTable;
		private readonly ListTable _listTable;

		public string ProjectDirectory { get; set; }

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
			_listTable.ParseListFile(labelContents, ProjectDirectory);
		}

		public void DestroyService()
		{
		}

		public void InitService(params object[] objects)
		{
			
		}

		public SymbolService()
		{
			_symbolTable = new SymbolTable();
			_listTable = new ListTable();
		}
	}
}
