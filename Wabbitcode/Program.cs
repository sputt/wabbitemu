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
            //frmSplash splashScreen = new frmSplash();
            //splashScreen.Show(); 
            Application.EnableVisualStyles();
            //Application.Run(new spriteEditor());
            Application.Run(new MainFormRedone(args));
            //Application.Run(new frmMain());

            //Application.Run(new frmMain());
            //splashScreen.Hide();
        }
    }
}