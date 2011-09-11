using System;
using Revsoft.Wabbitcode.Panels;
using Revsoft.Wabbitcode.Interface;

namespace Revsoft.Wabbitcode.Services
{
	interface IDocumentService : IService
	{
		Editor CreateDocument(string title);
		void OpenDocument();
		void OpenDocument(Editor doc, string filename);
		Editor OpenDocument(string filename);

		void OpenDocument(Editor doc);
	}
}
