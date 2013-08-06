using System.Collections.Generic;
using Microsoft.VisualBasic.ApplicationServices;
using System;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Services;

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
				//{ typeof(IDocumentService), typeof(DocumentService) },
				{ typeof(IAssemblerService), typeof(AssemblerService) },
				{ typeof(ISymbolService), typeof(SymbolService) },
				{ typeof(IParserService), typeof(ParserService) },
				{ typeof(IBackgroundAssemblerService), typeof(BackgroundAssemblerService) },
			};

			ServiceFactory.Instance = new ServiceFactory(dictionary);
		}

		/// <summary>
		/// We are responsible for creating the application's main form in this override.
		/// </summary>
		protected override void OnCreateMainForm()
		{
			// Create an instance of the main form and set it in the application;
			// but don't try to run it.
			string[] args = new string[CommandLineArgs.Count];
			CommandLineArgs.CopyTo(args, 0);
			MainForm = new MainFormRedone(args);
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
			string[] args = new string[eventArgs.CommandLine.Count];
			eventArgs.CommandLine.CopyTo(args, 0);

			MainFormRedone form = MainForm as MainFormRedone;
			if (form == null)
			{
				throw new InvalidCastException("Main window is not the correct type");
			}

			// Need to use invoke to b/c this is being called from another thread.
			MainForm.Invoke(() => form.ProcessParameters(args));
		}
	}
}