namespace Revsoft.Wabbitcode
{
    using System;
    using System.Collections;
    using System.IO;
    using System.Windows.Forms;
    using System.Xml;

    using Revsoft.Wabbitcode.Properties;

    public class NewProject
    {
        private readonly string projectFile;

        public NewProject(string projectFile)
        {
            this.projectFile = projectFile;
        }
    }
}