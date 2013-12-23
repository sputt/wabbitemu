using Revsoft.Wabbitcode.Services.Symbols;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
	public interface ISymbolService : IService
	{
	    SymbolTable SymbolTable { get; }
		ListTable ListTable { get; }
		void ParseSymbolFile(string symbolContents);
		void ParseListFile(string labelContents);
	}
}