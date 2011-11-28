using System;
using System.Linq;
using System.Windows;
using Microsoft.VisualBasic.ApplicationServices;

namespace Revsoft.Wabbitcode
{
    public class EntryPoint
    {
        private EntryPoint() { }

        [STAThread]
        public static void Main(string[] args)
        {
            AppBase manager = new AppBase();
            manager.Run(args);
        }
    }

    public class SingleInstanceApplication : Application
    {
        protected override void OnStartup(System.Windows.StartupEventArgs e)
        {
            base.OnStartup(e);

            // Create and show the application's main window
            this.MainWindow = new MainWindow(e.Args);
            this.MainWindow.Show();
        }

        public void HandleArgs(string[] args)
        {
            ((MainWindow)this.MainWindow).HandleArgs(args);
        }

        public void Activate()
        {
            // Reactivate application's main window
            this.MainWindow.Activate();
        }
    }

    class AppBase : WindowsFormsApplicationBase
    {
        SingleInstanceApplication app;
        public AppBase()
        {
            // Make this a single-instance application
            this.IsSingleInstance = true;
            this.EnableVisualStyles = true;

            // There are some other things available in the VB application model, for
            // instance the shutdown style:
            this.ShutdownStyle = Microsoft.VisualBasic.ApplicationServices.ShutdownMode.AfterMainFormCloses;
        }

        protected override bool OnStartup(Microsoft.VisualBasic.ApplicationServices.StartupEventArgs e)
        {
            // First time app is launched
            app = new SingleInstanceApplication();
            app.Run();
            return false;
        }

        protected override void OnStartupNextInstance(StartupNextInstanceEventArgs eventArgs)
        {
            // Subsequent launches
            base.OnStartupNextInstance(eventArgs);
            app.HandleArgs(eventArgs.CommandLine.ToArray());
            app.Activate();
        }

        protected override void OnShutdown()
        {
            Properties.Settings.Default.Save();
            Properties.Editor.Default.Save();
            base.OnShutdown();
        }
    }
}
