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

        private void okButton_Click(object sender, EventArgs e)
        {
			if (currentConfig != null && !currentConfig.Equals(ProjectService.BuildConfigs[currentIndex]))
				if (MessageBox.Show("Would you like to save the current configuration?",
										"Save?", MessageBoxButtons.YesNo, MessageBoxIcon.None) == DialogResult.Yes)
					ProjectService.BuildConfigs[currentIndex] = currentConfig;
        }

        private void editButton_Click(object sender, EventArgs e)
        {
            
        }

        private void moveUp_Click(object sender, EventArgs e)
        {
            
        }

        private void moveDown_Click(object sender, EventArgs e)
        {
            
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
			UpdateStepOptions();
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

					inputBox.Text = step.InputFile;
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
    }
}