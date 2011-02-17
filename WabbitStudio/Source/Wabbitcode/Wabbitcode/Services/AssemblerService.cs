using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

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
            string originalDir = Path.GetDirectoryName(inputPath);
            Assembler.Instance.ClearIncludeDirs();
            Assembler.Instance.ClearDefines();
            Assembler.Instance.AddIncludeDir(originalDir);
            //if the user has some include directories we need to format them
            /*if (Properties.Settings.Default.includeDir != "")
            {
                string[] dirs = Properties.Settings.Default.includeDir.Split('\n');
                foreach (string dir in dirs)
                    if (dir != "")
                        Assembler.AddIncludeDirectory(dir);
            }*/
            Assembler.Instance.SetInputFile(inputPath);
            Assembler.Instance.SetOutputFile(outputPath);

            //Assembler.SetFlags(SPASM.AssemblyFlags.MODE_NORMAL | AssemblyFlags.MODE_LIST);
            //assemble that fucker
            Assembler.Instance.Assemble();

            string test = Assembler.Instance.GetOutput();
            DockingService.OutputWindow.AddText(test);
            return true;
        }

        internal static void InitAssembler()
        {

        }

        
    }
}
