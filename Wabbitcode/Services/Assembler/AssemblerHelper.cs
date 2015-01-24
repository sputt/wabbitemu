using System.Collections.Generic;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Utils;

namespace Revsoft.Wabbitcode.Services.Assembler
{
    internal static class AssemblerHelper
    {
        public static void SetupAssembler(IAssembler assembler, FilePath inputFile, FilePath outputFile,
            FilePath currentDirectory, IEnumerable<FilePath> includeDirs)
        {
            assembler.SetWorkingDirectory(currentDirectory);

            // include dirs
            assembler.AddIncludeDir(Application.StartupPath);
            foreach (string dir in includeDirs)
            {
                assembler.AddIncludeDir(dir);
            }

            // setup files
            assembler.SetInputFile(inputFile);
            assembler.SetOutputFile(outputFile);

            // set flags
            assembler.SetCaseSensitive(Settings.Default.CaseSensitive);
        }
    }
}