using System;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Properties;
using System.IO;

namespace Revsoft.Wabbitcode
{
    partial class MainFormRedone
    {
        /// <summary>
        /// This loads the recent file list from Properties and adds it to the recent file menu.
        /// </summary>
        private void getRecentFiles()
        {
            String line = Settings.Default.recentFiles;
            String[] list = line.Split('\n');

            foreach (String file in list)
            {
                if (string.IsNullOrEmpty(file))
                    continue;
                MenuItem button = new MenuItem(file, openRecentDoc);
                recentFilesMenuItem.MenuItems.Add(button);
            }
        }

        /// <summary>
        /// This opens the recend document clicked in the file menu.
        /// </summary>
        /// <param name="sender">This is the button object. This is casted to get which button was clicked.</param>
        /// <param name="e">Nobody cares about this arg.</param>
        private void openRecentDoc(object sender, EventArgs e)
        {
            var button = (MenuItem)sender;
            var doc = new newEditor();
            String fileName = button.Text;
            doc.Text = Path.GetFileName(fileName);
            doc.TabText = Path.GetFileName(fileName);
            doc.ToolTipText = fileName;
            doc.MdiParent = this;
            doc.editorBox.ShowLineNumbers = Settings.Default.lineNumbers;
            doc.openFile(fileName);
            doc.editorBox.Font = Settings.Default.editorFont;
            doc.Show(dockPanel);
            //doc.highlight();
        }

        /// <summary>
        /// This takes the string array of recent files, joins them into a large string and saves it in Properties.
        /// </summary>
        private void saveRecentFileList()
        {
            string list = null;
            foreach (String file in recentFileList)
                list += file + '\n';
            Settings.Default.recentFiles = list;
        }
    }
}
