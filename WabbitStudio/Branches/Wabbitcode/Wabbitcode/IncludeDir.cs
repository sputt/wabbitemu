using System.Collections.Specialized;
using Revsoft.Wabbitcode.Extensions;
using Revsoft.Wabbitcode.Properties;
using System;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using Revsoft.Wabbitcode.Services.Project;

namespace Revsoft.Wabbitcode
{
	public partial class IncludeDir : Form
	{
		private readonly IProject _project;

		public IncludeDir(IProject project)
		{
			InitializeComponent();

			_project = project;

			var directories = _project.IsInternal ?
									   Settings.Default.includeDirs.Cast<string>() :
									   _project.IncludeDirs;
			foreach (string dir in directories.Where(dir => !string.IsNullOrEmpty(dir)))
			{
				includeDirList.Items.Add(_project.IsInternal
					? dir
					: FileOperations.GetRelativePath(_project.ProjectFile, dir));
			}
		}

		private void addDirButton_Click(object sender, EventArgs e)
		{
			if (!_project.IsInternal)
			{
				includeDirBrowser.SelectedPath = _project.ProjectDirectory;
			}

			DialogResult include = includeDirBrowser.ShowDialog();
			if (include == DialogResult.OK && !includeDirList.Items.Contains(includeDirBrowser.SelectedPath))
			{
				includeDirList.Items.Add(includeDirBrowser.SelectedPath);
			}
		}

		private void deleteDirButton_Click(object sender, EventArgs e)
		{
			object index = includeDirList.SelectedItem;
			if (index != null)
			{
				includeDirList.Items.Remove(index);
			}
		}

		private void okButton_Click(object sender, EventArgs e)
		{
			if (_project.IsInternal)
			{
				Settings.Default.includeDirs = new StringCollection();
			}
			else
			{
				_project.IncludeDirs.Clear();
			}

			foreach (string includeDir in includeDirList.Items)
			{
				if (_project.IsInternal)
				{
					Settings.Default.includeDirs.Add(includeDir);
				}
				else
				{
					string path = Path.GetDirectoryName(_project.ProjectFile);
					if (string.IsNullOrEmpty(path))
					{
						continue;
					}
					string temp = new Uri(Path.Combine(path, includeDir)).AbsolutePath;
					_project.IncludeDirs.Add(temp);
				}
			}

			if (!_project.IsInternal)
			{
				_project.SaveProject();
			}
		}
	}
}