namespace Revsoft.Wabbitcode.Services.Project
{
    using System;
    using System.Collections.Generic;
    using System.Xml;

    public interface IBuildSystem
    {
        List<BuildConfig> BuildConfigs
        {
            get;
        }

        BuildConfig CurrentConfig
        {
            get;
            set;
        }

        int CurrentConfigIndex
        {
            get;
            set;
        }

        string MainFile
        {
            get;
        }

        string MainOutput
        {
            get;
        }

        string OutputText
        {
            get;
        }

        bool Build();

        void ReadXML(XmlTextReader reader);

        void WriteXML(XmlTextWriter writer);
    }
}