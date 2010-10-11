using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.VisualBasic.ApplicationServices;

namespace Revsoft.Wabbitcode
{
    class AppBase : WindowsFormsApplicationBase
    {
        public AppBase()
        {
            // Make this a single-instance application
            this.IsSingleInstance = true;
            this.EnableVisualStyles = true;

            // There are some other things available in the VB application model, for
            // instance the shutdown style:
            this.ShutdownStyle = Microsoft.VisualBasic.ApplicationServices.ShutdownMode.AfterMainFormCloses;

            // Add StartupNextInstance handler
            this.StartupNextInstance += new StartupNextInstanceEventHandler(this.SIApp_StartupNextInstance);
        }

        /// <summary>
        /// We are responsible for creating the application's main form in this override.
        /// </summary>
        protected override void OnCreateMainForm()
        {
            // Create an instance of the main form and set it in the application; 
            // but don't try to run it.
            string[] args = new string[this.CommandLineArgs.Count];
            this.CommandLineArgs.CopyTo(args, 0);
            this.MainForm = new MainFormRedone(args);
        }

        /// <summary>
        /// This is called for additional instances. The application model will call this 
        /// function, and terminate the additional instance when this returns.
        /// </summary>
        /// <param name="eventArgs"></param>
        protected void SIApp_StartupNextInstance(object sender, StartupNextInstanceEventArgs eventArgs)
        {
            // Copy the arguments to a string array
            string[] args = new string[eventArgs.CommandLine.Count];
            eventArgs.CommandLine.CopyTo(args, 0);

            // Create an argument array for the Invoke method
            object[] parameters = new object[2];
            parameters[0] = this.MainForm;
            parameters[1] = args;

            // Need to use invoke to b/c this is being called from another thread.
            this.MainForm.Invoke(new MainFormRedone.ProcessParametersDelegate(((MainFormRedone)this.MainForm).ProcessParameters), parameters);
        }
    }
}
