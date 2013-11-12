using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services.Interfaces
{
    public delegate void ParserProgressHandler(object sender, ParserProgressEventArgs e);
    public delegate void ParserFinishedHandler(object sender, ParserEventArgs e);

	public interface IParserService : IService
	{
		event ParserProgressHandler OnParserProgress;
        event ParserFinishedHandler OnParserFinished;

		/// <summary>
		/// Finds all references to the given text.
		/// </summary>
		/// <param name="file">Fully rooted path to the file</param>
		/// <param name="refString">String to find references to</param>
		List<Reference> FindAllReferencesInFile(string file, string refString);

		IEnumerable<IParserData> GetAllParserData();
		IEnumerable<IParserData> GetParserData(string referenceString, bool caseSensitive);

		void ParseFile(int hashCode, string filename);
	    void ParseFile(int hashCode, string file, string fileText);
		ParserInformation GetParserInfo(string fileName);
	}
}