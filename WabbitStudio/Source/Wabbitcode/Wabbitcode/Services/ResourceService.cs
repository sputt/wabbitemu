using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Resources;
using System.Reflection;
using System.IO;

namespace Revsoft.Wabbitcode.Services
{
    public static class ResourceService
    {
        /// <summary>
        /// Gets a resource embedded in the assembly
        /// </summary>
        /// <param name="name">Name of the resource. Must include dirs below Wabbitcode\Resources</param>
        /// <returns>Returns string of the extracted resource</returns>
        public static string GetResource(string name)
        {
            var asm = Assembly.GetExecutingAssembly();
            var stream = asm.GetManifestResourceStream("Revsoft.Wabbitcode.Resources." + name);
            var sr = new StreamReader(stream);
            return sr.ReadToEnd();
        }

    }
}
