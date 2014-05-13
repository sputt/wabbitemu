using System.Collections.Generic;
using System.Xml;
using Revsoft.Wabbitcode.Services.Interfaces;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Project
{
    public interface IBuildSystem
    {
        IAssemblerService AssemblerService { get; set; }

        IList<BuildConfig> BuildConfigs { get; }

        BuildConfig CurrentConfig { get; set; }

        int CurrentConfigIndex { get; set; }

        FilePath MainFile { get; }

        string OutputText { get; }

        FilePath ProjectOutput { get; set; }

        FilePath LabelOutput { get; set; }

        FilePath ListOutput { get; set; }

        void CreateDefaultConfigs();

        bool Build();

        void ReadXML(XmlTextReader reader);

        void WriteXML(XmlTextWriter writer);
    }
}