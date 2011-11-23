using System;
using Revsoft.Wabbitcode.Panels;
using Revsoft.Wabbitcode.Interface;

namespace Revsoft.Wabbitcode.Services
{
	interface IDocumentService : IService
	{
		Editor CreateDocument(string title);

		Editor OpenDocument(string filename);
		void OpenDocument(Editor doc);
		void OpenDocument(Editor doc, string filename);

	}
}
