using System;
using System.Threading.Tasks;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Utils;

namespace Revsoft.Wabbitcode.Services
{
	[ServiceDependency(typeof(IProjectService))]
	[ServiceDependency(typeof(IAssemblerService))]
    [ServiceDependency(typeof(IStatusBarService))]
	public class BackgroundAssemblerService : IBackgroundAssemblerService
	{
		#region Private Members

		private volatile bool _isAssembling;
		private volatile bool _restartAssembling;
		private readonly IAssemblerService _assemblerService;
		private readonly IProjectService _projectService;
	    private readonly IStatusBarService _statusBarService;

		#endregion

		#region Events

		public event EventHandler<AssemblyFinishEventArgs> BackgroundAssemblerComplete;

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

	    public void CountCode(string codeInfoLines)
	    {
	        Task.Factory.StartNew(() =>
	        {
	            var info = _assemblerService.CountCode(codeInfoLines);
	            _statusBarService.SetCodeCountInfo(info);
	        });
	    }

		#endregion

		private void AssemblerFinished(object sender, AssemblyFinishEventArgs e)
		{
		    if (BackgroundAssemblerComplete != null)
		    {
		        BackgroundAssemblerComplete(sender, e);
		    }
		}

		#region IService

		public BackgroundAssemblerService(IAssemblerService assemblerService, IProjectService projectService, 
            IStatusBarService statusBarService)
		{
			_projectService = projectService;
			_assemblerService = assemblerService;
		    _statusBarService = statusBarService;
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
