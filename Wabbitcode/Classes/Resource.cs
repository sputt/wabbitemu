using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Reflection;

namespace Revsoft.Wabbitcode.Classes
{
	public static class Resources
	{
		public static void GetResource(string resourceName, string outputFile)
		{
			if (File.Exists(outputFile) && !Properties.Settings.Default.firstRun)
				return;
			BinaryWriter writeSPASM = null;
			try
			{
				Assembly asm = Assembly.GetExecutingAssembly();
				Stream spasm = asm.GetManifestResourceStream("Revsoft.Wabbitcode.Resources." + resourceName);
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
	}
}
