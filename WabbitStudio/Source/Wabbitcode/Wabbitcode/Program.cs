using System;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            AppBase appBase = new AppBase();
            appBase.Run(args);
        }
    }
}