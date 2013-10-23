using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services.Interface
{
	public interface IParserService : IService
	{
		event ParserService.ParserProgress OnParserProgress;

		/// <summary>
		/// Finds all references to the given text.
		/// </summary>
		/// <param name="file">Fully rooted path to the file</param>
		/// <param name="refString">String to find references to</param>
		List<Reference> FindAllReferencesInFile(string file, string refString);

		IEnumerable<IParserData> GetAllParserData();
		IEnumerable<IParserData> GetParserData(string referenceString, bool caseSensitive);

		void ParseFile(int hashCode, string filename);
	    ParserInformation ParseFile(int hashCode, string file, string lines);
		ParserInformation GetParserInfo(string fileName);
	}
}