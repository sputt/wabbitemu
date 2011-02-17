using System;
using System.Collections;
using System.IO;
using System.Xml;
using Revsoft.Wabbitcode.Properties;

namespace Revsoft.Wabbitcode
{
    public class NewProject
    {
        public NewProject(string projectFile)
        {
            this.projectFile = projectFile;
        }
        private readonly string projectFile;
        
    }
}