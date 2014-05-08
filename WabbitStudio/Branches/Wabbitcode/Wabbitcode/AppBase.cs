using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualBasic.ApplicationServices;
using System;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.GUI;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Interfaces;

namespace Revsoft.Wabbitcode
{
	public class AppBase : WindowsFormsApplicationBase
	{
		public AppBase()
		{
			// Make this a single-instance application
			IsSingleInstance = true;
			EnableVisualStyles = true;

			// There are some other things available in the VB application model, for
			// instance the shutdown style:
			ShutdownStyle = ShutdownMode.AfterMainFormCloses;

			// Add StartupNextInstance handler
			StartupNextInstance += SIApp_StartupNextInstance;

			IDictionary<Type, Type> dictionary = new Dictionary<Type, Type>
			{
				{ typeof(IDockingService), typeof(DockingService) },
				{ typeof(IProjectService), typeof(ProjectService) },
				{ typeof(IAssemblerService), typeof(AssemblerService) },
				{ typeof(ISymbolService), typeof(SymbolService) },
				{ typeof(IParserService), typeof(ParserService) },
				{ typeof(IBackgroundAssemblerService), typeof(BackgroundAssemblerService) },
                { typeof(IFileService), typeof(FileService) },
                { typeof(IStatusBarService), typeof(StatusBarService) },
                { typeof(IToolBarService), typeof(ToolBarService) },
                { typeof(IDebuggerService), typeof(DebuggerService) },
                { typeof(IMenuService), typeof(MenuService) },
                { typeof(IPluginService), typeof(PluginService) },
			};

			ServiceFactory.Instance = new ServiceFactory(dictionary);
            LoggingService.Instance = new LoggingService();
		}

		/// <summary>
		/// We are responsible for creating the application's main form in this override.
		/// </summary>
		protected override void OnCreateMainForm()
		{
			// Create an instance of the main form and set it in the application;
			// but don't try to run it.
			MainForm = new MainForm(CommandLineArgs.ToArray());
		}

		/// <summary>
		/// This is called for additional instances. The application model will call this
		/// function, and terminate the additional instance when this returns.
		/// </summary>
		/// <param name="sender" />
		/// <param name="eventArgs"></param>
		private void SIApp_StartupNextInstance(object sender, StartupNextInstanceEventArgs eventArgs)
		{
			// Copy the arguments to a string array
		    string[] args = eventArgs.CommandLine.ToArray();

			MainForm form = MainForm as MainForm;
			if (form == null)
			{
				throw new InvalidCastException("Main window is not the correct type");
			}

			// Need to use invoke to b/c this is being called from another thread.
			MainForm.Invoke(() => form.ProcessParameters(args));
		}
	}
}