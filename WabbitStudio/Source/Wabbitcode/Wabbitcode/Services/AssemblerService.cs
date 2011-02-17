using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services
{
    public static class AssemblerService
    {
        /// <summary>
        /// Assembles a file using SPASM and optionally sends it to an instance of wabbitemu
        /// </summary>
        /// <param name="inputPath">Input file to send as the first param to spasm</param>
        /// <param name="outputPath">Output file for spasm to create</param>
        /// <param name="sendFileEmu">Indicates whether the file should be attempted to be sent to wabbitemu</param>
        /// <returns>True if successful else false</returns>
        internal static bool AssembleFile(string inputPath, string outputPath, bool sendFileEmu)
        {
            throw new NotImplementedException();
        }
    }
}
