using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;
using Revsoft.Wabbitcode.Services.Parser;
using System.Windows.Forms;

namespace Revsoft.Wabbitcode.Services.NewParser
{
    public static class NewParser
    {
        public static ParserInformation ParseFile(string file)
        {
            string lines = null;
            StreamReader reader = null;
#if !DEBUG
			try
			{
#endif
            reader = new StreamReader(file);
            lines = reader.ReadToEnd();
#if DEBUG
            reader.Close();
#endif
            return ParseFile(file, lines.Split('\n'));
#if !DEBUG
			}
			catch (FileNotFoundException ex)
			{
				DialogResult result = MessageBox.Show(ex.FileName + " not found, would you like to remove it from the project?",
					"File not found", MessageBoxButtons.YesNo, MessageBoxIcon.None);
				if (result == DialogResult.Yes)
					ProjectService.DeleteFile(file);
				return null;
			}
			catch (Exception ex)
			{
				MessageBox.Show("File: " + file + "\n" + ex.ToString());
				return null;
			}
			finally
			{
				if (reader != null)
					reader.Close();
			}
#endif
        }

        private static ParserInformation ParseFile(string file, string[] lines)
        {
			ParserInformation info = new ParserInformation(file);
            var fileData = new List<ParserLine>();
            foreach (var line in lines)
            {
				var lineData = ParserLine.ParseLine(line, info);
				fileData.Add(lineData);
            }
            return info;
        }
    }
}
