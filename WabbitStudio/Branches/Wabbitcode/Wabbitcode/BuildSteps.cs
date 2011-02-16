using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Xml;
using Revsoft.Wabbitcode.Classes;
using Revsoft.Wabbitcode.Services.Project;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode
{
    public partial class BuildSteps : Form
    {
		private BuildConfig currentConfig;
        public BuildSteps()
        {
            InitializeComponent();
            GetBuildConfigs();
			configBox.SelectedIndex = ProjectService.CurrentConfigIndex;
        }

        private void GetBuildConfigs()
        {
			foreach (BuildConfig config in ProjectService.BuildConfigs)
				configBox.Items.Add(config);
        }

        private void PopulateListBox()
        {
            buildSeqList.Items.Clear();
			currentConfig.SortSteps();
            foreach (IBuildStep step in currentConfig.Steps)
				buildSeqList.Items.Add(step);
			buildSeqList.SelectedIndex = currentConfig.Steps.Count - 1;
        }

        private void addDirButton_Click(object sender, EventArgs e)
        {
			int count = buildSeqList.Items.Count;
			string fileName = ProjectService.ProjectName + ".asm";
			IBuildStep stepToAdd = new InternalBuildStep(count, StepType.Assemble, fileName , Path.ChangeExtension(fileName, "8xk"));
			currentConfig.Steps.Add(stepToAdd);
			buildSeqList.Items.Insert(count, stepToAdd);
        }

        private void deleteDirButton_Click(object sender, EventArgs e)
        {
            IBuildStep selectedItem = (IBuildStep) buildSeqList.SelectedItem;
            if (selectedItem == null)
                return;
			bool found = currentConfig.Steps.Remove(selectedItem);
			buildSeqList.Items.Remove(selectedItem);
        }

        private bool needsSave = false;
        private void okButton_Click(object sender, EventArgs e)
        {
			if ((currentConfig != null && !currentConfig.Equals(ProjectService.BuildConfigs[currentIndex])) || needsSave)
				if (MessageBox.Show("Would you like to save the current configuration?",
										"Save?", MessageBoxButtons.YesNo, MessageBoxIcon.None) == DialogResult.Yes)
					ProjectService.BuildConfigs[currentIndex] = currentConfig;
        }

        private void editButton_Click(object sender, EventArgs e)
        {
            needsSave = true;
        }

        private void moveUp_Click(object sender, EventArgs e)
        {
            needsSave = true;
        }

        private void moveDown_Click(object sender, EventArgs e)
        {
            needsSave = true;
        }

		int currentIndex = 0;
        private void configBox_SelectedIndexChanged(object sender, EventArgs e)
        {
			if (currentConfig != null && !currentConfig.Equals(ProjectService.BuildConfigs[currentIndex]))
				if (MessageBox.Show("Would you like to save the current configuration?",
										"Save?", MessageBoxButtons.YesNo, MessageBoxIcon.None) == DialogResult.Yes)
					ProjectService.BuildConfigs[currentIndex] = currentConfig;
			currentIndex = configBox.SelectedIndex;
			currentConfig = (BuildConfig)ProjectService.BuildConfigs[currentIndex].Clone();
			PopulateListBox();
			UpdateStepOptions();
        }

        /// <summary>
        /// This is a hack to remember the config so we can save it
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void configBox_DropDown(object sender, EventArgs e)
        {

        }

		private void stepTypeBox_SelectedIndexChanged(object sender, EventArgs e)
		{
            IBuildStep step = null;
            if (stepTypeBox.SelectedIndex == 0 && buildSeqList.SelectedItem.GetType() != typeof(InternalBuildStep))
                step = new InternalBuildStep(buildSeqList.SelectedIndex, StepType.Assemble,
                                        Path.ChangeExtension(ProjectService.ProjectFile, ".asm"), Path.ChangeExtension(ProjectService.ProjectFile, ".8xk"));
            else if (stepTypeBox.SelectedIndex == 1 && buildSeqList.SelectedItem.GetType() != typeof(ExternalBuildStep))
                step = new ExternalBuildStep(buildSeqList.SelectedIndex, "cmd.exe", "");
            if (step != null)
            {
                currentConfig.Steps[configBox.SelectedIndex] = step;
                buildSeqList.Items[buildSeqList.SelectedIndex] = step;
                UpdateStepOptions();
                needsSave = true;
            }
		}

		private void buildSeqList_SelectedIndexChanged(object sender, EventArgs e)
		{
			UpdateStepOptions();
		}

		private void UpdateStepOptions()
		{
			if (buildSeqList.SelectedIndex == -1)
			{
				stepOptionsBox.Enabled = false;
				return;
			}
			else
				stepOptionsBox.Enabled = true;
			Type stepType = buildSeqList.SelectedItem.GetType();
			if (stepType == typeof(InternalBuildStep))
			{
				stepTypeBox.SelectedIndex = 0;
			}
			else if (stepType == typeof(ExternalBuildStep))
			{
				stepTypeBox.SelectedIndex = 1;
			}
			IBuildStep step = (IBuildStep)buildSeqList.SelectedItem;
			//HACK: this just doesnt feel right :?
			switch (stepTypeBox.SelectedIndex)
			{
				case 0:
					InternalBuildStep intStep = (InternalBuildStep)step;
					inputLabel.Text = "Input File:";
					outputLabel.Enabled = true;
					outputBox.Enabled = true;
					actionBox.Visible = true;
					actionLabel.Visible = true;

                    inputBox.Text = intStep.InputFile;
                    outputBox.Text = intStep.OutputFile;
					actionBox.SelectedIndex = (int)intStep.StepType;
					break;
				case 1:
					inputLabel.Text = "Command:";
					outputLabel.Enabled = false;
					outputBox.Enabled = false;
					actionBox.Visible = false;
					actionLabel.Visible = false;

					inputBox.Text = step.InputFile;
					break;
			}
		}

        private void inputBox_TextChanged(object sender, EventArgs e)
        {
            IBuildStep step = (IBuildStep)buildSeqList.SelectedItem;
            if (step.GetType() == typeof(InternalBuildStep))
                step.InputFile = FileOperations.NormalizePath(Path.Combine(ProjectService.ProjectDirectory, inputBox.Text));
            else if (step.GetType() == typeof(ExternalBuildStep))
                step.InputFile = inputBox.Text;
            needsSave = true;
        }

        private void outputBox_TextChanged(object sender, EventArgs e)
        {
            InternalBuildStep step = (InternalBuildStep)buildSeqList.SelectedItem;
            step.OutputFile = FileOperations.NormalizePath(Path.Combine(ProjectService.ProjectDirectory, outputBox.Text));
            needsSave = true;
        }

        private void actionBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            InternalBuildStep step = (InternalBuildStep)buildSeqList.SelectedItem;
            step.StepType = (StepType)actionBox.SelectedIndex;
            buildSeqList.Items[buildSeqList.SelectedIndex] = step;
            currentConfig.Steps[buildSeqList.SelectedIndex] = step;
            UpdateStepOptions();
            needsSave = true;
        }

        private void browseInput_Click(object sender, EventArgs e)
        {
            inputBox.Text = DoOpenFileDialog();
        }

        private void browseOutput_Click(object sender, EventArgs e)
        {
            outputBox.Text = DoOpenFileDialog();
        }

        private string DoOpenFileDialog()
        {
            OpenFileDialog openFileDialog = new OpenFileDialog()
            {
                CheckFileExists = true,
                DefaultExt = "*.asm",
                Filter = "All Know File Types | *.asm; *.z80; *.inc; |Assembly Files (*.asm)|*.asm|*.z80" +
                           " Assembly Files (*.z80)|*.z80|Include Files (*.inc)|*.inc|All Files(*.*)|*.*",
                FilterIndex = 0,
                Multiselect = false,
                RestoreDirectory = true,
                Title = "Add Existing File",
            };
            DialogResult result = openFileDialog.ShowDialog();
            return result != DialogResult.OK ? null : openFileDialog.FileName;
        }
    }
}