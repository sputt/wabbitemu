using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Docking_Windows;
using System.Text;
using System.Runtime.InteropServices;

namespace Revsoft.Wabbitcode.Classes
{
	//TODO: THIS ENTIRE FILE IS DEPRECATED
	//NEEDS TO BE REMOVED
	//PROGRESS HAS BEGUN!
    static class GlobalClass
    {
		public static List<Errors> errorsInFiles { get; set; }
		public static List<string> macros = new List<string>();
		public static int currentMacro = -1;

        public static void GetResource(string resourceName, string outputFile)
        {
            if (File.Exists(outputFile) && !Properties.Settings.Default.firstRun)
                return;
            BinaryWriter writeSPASM = null;
            try
            {
                Assembly asm = Assembly.GetExecutingAssembly();
                Stream spasm = asm.GetManifestResourceStream(resourceName);
                FileStream fs = new FileStream(outputFile, FileMode.Create, FileAccess.Write);
                writeSPASM = new BinaryWriter(fs);
                if (spasm != null)
                {
                    long temp = spasm.Length;
                    int length = Convert.ToInt32(temp);
                    var buffer = new byte[temp];
                    spasm.Read(buffer, 0, length);
                    writeSPASM.Write(buffer);
                }
            }
            catch (Exception ex)
            {
                //errorConsoleBox.Text += ex.ToString() + '\n';
                MessageBox.Show("Error: " + ex);
            }
            finally
            {
                if (writeSPASM != null)
                {
                    writeSPASM.Flush();
                    writeSPASM.Close();
                }
            }
        }
		/*public static string RelativePath(string absolutePath, string relativeTo)        
		{            
			string[] absoluteDirectories = absolutePath.Split('\\');            
			string[] relativeDirectories = relativeTo.Split('\\');           
			//Get the shortest of the two paths
			int length = absoluteDirectories.Length < relativeDirectories.Length ? absoluteDirectories.Length : relativeDirectories.Length;
			//Use to determine where in the loop we exited
			int lastCommonRoot = -1;
			int index;            
			//Find common root            
			for (index = 0; index < length; index++)
				if (absoluteDirectories[index] == relativeDirectories[index])                    
					lastCommonRoot = index;                
				else                    
					break;            
			//If we didn't find a common prefix then throw            
			if (lastCommonRoot == -1)
				throw new ArgumentException("Paths do not have a common base");
			//Build up the relative path            
			StringBuilder relativePath = new StringBuilder(); 
			//Add on the ..            
			for (index = lastCommonRoot + 1; index < absoluteDirectories.Length; index++)
				if (absoluteDirectories[index].Length > 0)
					relativePath.Append("..\\");            //Add on the folders            
			for (index = lastCommonRoot + 1; index < relativeDirectories.Length - 1; index++)                
				relativePath.Append(relativeDirectories[index] + "\\");            
			relativePath.Append(relativeDirectories[relativeDirectories.Length - 1]);            
			return relativePath.ToString();
		}*/
    }
    
		
	class Errors
    {
        public string file;
        public int lineNum;
        public bool isWarning;
        public string description;
        public ToolTip toolTip;
        public Errors(string file, int lineNum, string description, bool isWarning)
        {
            this.file = file;
            this.lineNum = lineNum;
            this.description = description;
            this.isWarning = isWarning;
            toolTip = new ToolTip {ToolTipTitle = description};
        }
    }
}
