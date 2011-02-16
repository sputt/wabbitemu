using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Revsoft.Wabbitcode.Services
{
    public static class RecentFileService
    {
        static List<string> recentFileList = new List<string>();

        internal static void AddRecentFile(string filePath)
        {
            if (recentFileList.Contains(filePath))
                return;
            recentFileList.Add(filePath);
        }



        internal static void SaveRecentFileList()
        {
            
        }

        internal static void GetRecentFiles()
        {
            
        }
    }
}
