using System;
using System.Threading.Tasks;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interface;

namespace Revsoft.Wabbitcode.Services
{
	[ServiceDependency(typeof(IProjectService))]
	[ServiceDependency(typeof(IAssemblerService))]
	public class BackgroundAssemblerService : IBackgroundAssemblerService
	{
		#region Private Static Members

		private volatile bool _isAssembling;
		private volatile bool _restartAssembling;
		private readonly IAssemblerService _assemblerService;
		private readonly IProjectService _projectService;

		#endregion

		#region Events

		public delegate void BackgroundAssemblerComplete(object sender, EventArgs e);
		public event BackgroundAssemblerComplete OnBackgroundAssemblerComplete;

		#endregion

		#region Public Methods

		public void RequestAssemble()
		{
			if (_isAssembling)
			{
				_restartAssembling = true;
				return;
			}

			_isAssembling = true;
			Task.Factory.StartNew(() =>
			{
				if (_projectService.Project == null || _projectService.Project.IsInternal)
				{
					return;
				}
				_assemblerService.AssemblerProjectFinished += AssemblerFinished;
				_assemblerService.AssembleProject(_projectService.Project);

				_isAssembling = false;

				if (!_restartAssembling)
				{
					return;
				}

				_restartAssembling = false;
				RequestAssemble();
			});
		}

		//private void CodeCheckAssemblerFinished(object sender, AssemblyFinishEventArgs e)
		//{
		//	foreach (Editor doc in DockingService.Documents)
		//	{
		//		doc.EditorBox.Document.MarkerStrategy.RemoveAll(marker => marker != null && marker.Tag == "Code Check");
		//	}

		//	foreach (var item in e.Output.ParsedErrors)
		//	{
		//		if (string.Equals(item.File, _fileName, StringComparison.OrdinalIgnoreCase))
		//		{
		//			Color underlineColor = item.IsWarning ? Color.Yellow : Color.Red;
		//			Errors errors = item;
		//			_mainForm.Invoke(() =>
		//			{
		//				AddSquiggleLine(this, errors.LineNum, underlineColor, errors.Description);
		//				UpdateDocument(errors.LineNum);
		//			});
		//		}
		//		else
		//		{
		//			// because we are not thread safe, its possible that we close the editor as this is going
		//			var docsList = DockingService.Documents.Select(s => s).ToList();
		//			foreach (Editor doc in docsList)
		//			{
		//				if (string.Compare(item.File, doc.FileName, true) == 0)
		//				{
		//					Color underlineColor = item.IsWarning ? Color.Yellow : Color.Red;
		//					Editor newEditor = doc;
		//					Errors errors = item;
		//					_mainForm.Invoke(() =>
		//					{
		//						AddSquiggleLine(newEditor, errors.LineNum, underlineColor, errors.Description);
		//						newEditor.UpdateDocument(errors.LineNum);
		//					});
		//				}
		//			}
		//		}
		//	}
		//}

		#endregion

		private static void AssemblerFinished(object sender, AssemblyFinishProjectEventArgs e)
		{
			
		}

		#region IService

		public BackgroundAssemblerService(IAssemblerService assemblerService, IProjectService projectService)
		{
			_projectService = projectService;
			_assemblerService = assemblerService;
		}

		public void DestroyService()
		{
			_assemblerService.DestroyService();
		}

		public void InitService(params object[] objects)
		{

		}

		#endregion
	}
}
