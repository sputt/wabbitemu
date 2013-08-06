﻿using System.Collections.Generic;
using Revsoft.Wabbitcode.Services.Interface;
using Revsoft.Wabbitcode.Services.Parser;

namespace Revsoft.Wabbitcode.Services
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

		void ParseFile(int hashCode, string filename);
		ParserInformation GetParserInfo(string fileName);
	}
}