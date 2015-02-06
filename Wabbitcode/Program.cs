using System.Configuration;
using Microsoft.Practices.Unity;
using Revsoft.Wabbitcode.EditorExtensions;
using Revsoft.Wabbitcode.GUI.ToolBars;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;
using Revsoft.Wabbitcode.Services.Assembler;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Services.Parser;
using Revsoft.Wabbitcode.TextEditor;
using Revsoft.Wabbitcode.Utils;
using System;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace Revsoft.Wabbitcode
{
    internal static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        private static void Main(string[] args)
        {
            try
            {
                if (Settings.Default.FirstRun)
                {
                    Settings.Default.Upgrade();
                    Settings.Default.FirstRun = false;
                    Settings.Default.Save();
                }
            }
            catch (ConfigurationErrorsException ex)
            {
                DockingService.ShowError("Error upgrading settings", ex);
            }

            Application.EnableVisualStyles();

            DockPanel panel = new WabbitcodeDockPanel();
            StatusStrip statusBar = new WabbitcodeStatusBar();
            ToolStripContainer toolStripContainer = new WabbitcodeToolStripContainer(statusBar, panel);
            Task.Factory.StartNew(() => InitializeDependencies(panel, statusBar, toolStripContainer), TaskCreationOptions.PreferFairness);
            Task.Factory.StartNew(() =>
            {
                FileLocations.InitDirs();
                FileLocations.InitFiles();
                HighlightingUtils.MakeHighlightingFile();
            });

            Task.Factory.StartNew(() =>
            {
                if (!UpdateService.CheckForUpdate())
                {
                    return;
                }

                var result = MessageBox.Show("New version available. Download now?", "Update Available", MessageBoxButtons.YesNo, MessageBoxIcon.None);
                if (result != DialogResult.Yes)
                {
                    return;
                }

                UpdateService.StartUpdater();
                Application.Exit();
            });

            int numErrors = 0;
            AppBase appBase = new AppBase(toolStripContainer);
#if !DEBUG
            try
            {
#endif
                appBase.Run(args);
#if !DEBUG
            }
            catch (Exception ex)
            {
                numErrors++;
                DockingService.ShowError("Unhandled exception occurred. Please report this to the developers", ex);
            }
#endif

            if (numErrors == 0)
            {
                return;
            }

            do
            {
                try
                {
                    appBase.DoEvents();
                }
                catch (Exception ex)
                {
                    numErrors++;
                    DockingService.ShowError("Unhandled exception occurred. Please report this to the developers", ex);
                }
            } while (numErrors < 5);
        }

        private static void InitializeDependencies(DockPanel panel, StatusStrip statusBar, ToolStripContainer toolStripContainer)
        {
            // services
            DependencyFactory.RegisterType<IDockingService, DockingService>(new InjectionConstructor(panel, toolStripContainer));
            DependencyFactory.RegisterType<IStatusBarService, StatusBarService>(new InjectionConstructor(statusBar));
            DependencyFactory.RegisterType<IMenuService, MenuService>(new InjectionConstructor(toolStripContainer.TopToolStripPanel));
            DependencyFactory.RegisterType<IToolBarService, ToolBarService>(new InjectionConstructor(toolStripContainer.TopToolStripPanel));
            DependencyFactory.RegisterType<IAssemblerService, AssemblerService>();
            DependencyFactory.RegisterType<IBackgroundAssemblerService, BackgroundAssemblerService>(new ContainerControlledLifetimeManager());
            DependencyFactory.RegisterType<IProjectService, ProjectService>();
            DependencyFactory.RegisterType<IParserService, ParserService>();
            DependencyFactory.RegisterType<ISymbolService, SymbolService>();
            DependencyFactory.RegisterType<IFileService, FileService>();
            DependencyFactory.RegisterType<IDebuggerService, DebuggerService>();
            DependencyFactory.RegisterType<IPluginService, PluginService>();
            DependencyFactory.RegisterType<IMacroService, MacroService>();
            // factories
            DependencyFactory.RegisterType<IAssemblerFactory, AssemblerFactory>();
            DependencyFactory.RegisterType<IParserFactory, ParserFactory>();
            DependencyFactory.RegisterType<IFileTypeMethodFactory, FileTypeMethodFactory>();
            DependencyFactory.RegisterType<ICodeCompletionFactory, CodeCompletionFactory>();
        }
    }
}