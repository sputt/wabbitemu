using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

namespace Revsoft.Wabbitcode.Resources
{
    public static class Resources
    {
        // This makes sure we only extract stuff once per session...
        private static readonly List<string> ExtractedResources = new List<string>();

        /// <summary>
        /// Extracts a resource from the executing assembly.
        /// </summary>
        /// <param name="resourceName">The name of the resource to extract. The name is already prefixed with 
        /// Revsoft.Wabbitcode.Resources. </param>
        /// <param name="outputFile">The fully qualified path to extract the resource to</param>
        public static void GetResource(string resourceName, string outputFile)
        {
            if (ExtractedResources.Contains(resourceName.ToUpper()) && File.Exists(outputFile))
            {
                return;
            }

            Stream spasm = null;
            try
            {
                Assembly asm = Assembly.GetExecutingAssembly();
                spasm = asm.GetManifestResourceStream("Revsoft.Wabbitcode.Resources." + resourceName);
                FileStream fs = new FileStream(outputFile, FileMode.Create, FileAccess.Write);
                using (BinaryWriter writeSPASM = new BinaryWriter(fs))
                {
                    if (spasm != null)
                    {
                        long temp = spasm.Length;
                        int length = Convert.ToInt32(temp);
                        var buffer = new byte[temp];
                        spasm.Read(buffer, 0, length);
                        writeSPASM.Write(buffer);
                        ExtractedResources.Add(resourceName.ToUpper());
                    }
                    else
                    {
                        throw new Exception("Unable to find resource to extract");
                    }
                }
            }
            finally
            {
                if (spasm != null)
                {
                    spasm.Dispose();
                }
            }
        }
    }
}