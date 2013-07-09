namespace Revsoft.Wabbitcode
{
    using Revsoft.Wabbitcode.Services;
    using Revsoft.Wabbitcode.Services.Project;
    using System;
    using System.IO;
    using System.Windows.Forms;

    public partial class BuildSteps : Form
    {
        private BuildConfig currentConfig;
        private int currentIndex = 0;
        private bool isInternal = false;
        private bool needsSave = false;

        public BuildSteps()
        {
            InitializeComponent();
            this.GetBuildConfigs();
            isInternal = true;
            configBox.SelectedIndex = ProjectService.CurrentConfigIndex;
            isInternal = false;
        }

        private void actionBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            InternalBuildStep step = (InternalBuildStep)buildSeqList.SelectedItem;
            step.StepType = (BuildStepType)actionBox.SelectedIndex;
            buildSeqList.Items[buildSeqList.SelectedIndex] = step;
            this.currentConfig.Steps[buildSeqList.SelectedIndex] = step;
            this.UpdateStepOptions();
            needsSave = !isInternal;
        }

        private void addDirButton_Click(object sender, EventArgs e)
        {
            int count = buildSeqList.Items.Count;
            string fileName = ProjectService.ProjectName + ".asm";
            IBuildStep stepToAdd = new InternalBuildStep(count, BuildStepType.Assemble, fileName, Path.ChangeExtension(fileName, "8xk"));
            this.currentConfig.Steps.Add(stepToAdd);
            buildSeqList.Items.Insert(count, stepToAdd);
            needsSave = true;
        }

        private void browseInput_Click(object sender, EventArgs e)
        {
            inputBox.Text = this.DoOpenFileDialog();
        }

        private void browseOutput_Click(object sender, EventArgs e)
        {
            outputBox.Text = this.DoOpenFileDialog();
        }

        private void buildSeqList_SelectedIndexChanged(object sender, EventArgs e)
        {
            this.UpdateStepOptions();
        }

        /// <summary>
        /// This is a hack to remember the config so we can save it
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void configBox_DropDown(object sender, EventArgs e)
        {
        }

        private void configBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (this.currentConfig != null && !this.currentConfig.Equals(ProjectService.BuildConfigs[currentIndex]))
            {
                if (
                    MessageBox.Show(this,
                        "Would you like to save the current configuration?",
                        "Save?",
                        MessageBoxButtons.YesNo,
                        MessageBoxIcon.None) == DialogResult.Yes)
                {
                    ProjectService.BuildConfigs[currentIndex] = currentConfig;
                }
            }

            currentIndex = configBox.SelectedIndex;
            this.currentConfig = (BuildConfig)ProjectService.BuildConfigs[currentIndex].Clone();
            this.PopulateListBox();
            this.UpdateStepOptions();
        }

        private void deleteDirButton_Click(object sender, EventArgs e)
        {
            IBuildStep selectedItem = (IBuildStep)buildSeqList.SelectedItem;
            if (selectedItem == null)
            {
                return;
            }

            bool found = this.currentConfig.Steps.Remove(selectedItem);
            buildSeqList.Items.Remove(selectedItem);
            needsSave = true;
        }

        private string DoOpenFileDialog()
        {
            bool isExternal = buildSeqList.SelectedItem is ExternalBuildStep;
            string defExt = isExternal ? "*.bat" : "*.asm";
            string filter = isExternal ? "All Know File Types | *.bat; *.exe; |Executable (*.exe)|*.exe|Batch Files (*.bat)|*.bat|All Files (*.*)|*.*" :
                            "All Know File Types | *.asm; *.z80; *.inc; |Assembly Files (*.asm)|*.asm|*.z80" +
                            " Assembly Files (*.z80)|*.z80|Include Files (*.inc)|*.inc|All Files (*.*)|*.*";
            string title = isExternal ? "Program to run" : "Add existing file";
            string filePath = null;
            using (OpenFileDialog openFileDialog = new OpenFileDialog()
            {
                CheckFileExists = true,
                DefaultExt = defExt,
                Filter = filter,
                FilterIndex = 0,
                Multiselect = false,
                RestoreDirectory = true,
                Title = title,
            })
            {
                DialogResult result = openFileDialog.ShowDialog();
                if (result == DialogResult.OK)
                {
                    filePath = openFileDialog.FileName;
                }
            }

            return filePath;
        }

        private void editButton_Click(object sender, EventArgs e)
        {
            needsSave = true;
        }

        private void GetBuildConfigs()
        {
            foreach (BuildConfig config in ProjectService.BuildConfigs)
            {
                configBox.Items.Add(config);
            }

            if (configBox.Items.Count == 0)
            {
                ProjectService.BuildConfigs.Add(new BuildConfig("Debug"));
                ProjectService.BuildConfigs.Add(new BuildConfig("Release"));
                this.GetBuildConfigs();
            }
        }

        private void inputBox_TextChanged(object sender, EventArgs e)
        {
            IBuildStep step = (IBuildStep)buildSeqList.SelectedItem;
            if (step is InternalBuildStep)
            {
                step.InputFile = FileOperations.NormalizePath(Path.Combine(ProjectService.ProjectDirectory, inputBox.Text));
            }
            else if (step is ExternalBuildStep)
            {
                step.InputFile = inputBox.Text;
            }

            int index = buildSeqList.SelectedIndex;
            int selectIndex = inputBox.SelectionStart;

            // HACK: forces update of item text
            buildSeqList.Items.Remove(step);
            buildSeqList.Items.Insert(index, step);
            buildSeqList.SelectedIndex = index;
            inputBox.Focus();
            inputBox.SelectionStart = selectIndex;

            needsSave = !isInternal;
        }

        private void moveDown_Click(object sender, EventArgs e)
        {
            int index = buildSeqList.SelectedIndex;
            if (index == buildSeqList.Items.Count - 1)
            {
                return;
            }

            this.currentConfig.Steps[index].StepNumber++;
            for (int i = 0; i < this.currentConfig.Steps.Count; i++)
            {
                if (i != index)
                {
                    this.currentConfig.Steps[i].StepNumber--;
                }
            }

            index++;
            needsSave = true;
            IBuildStep selectedItem = (IBuildStep)buildSeqList.SelectedItem;
            buildSeqList.Items.Remove(selectedItem);
            buildSeqList.Items.Insert(index, selectedItem);
            buildSeqList.SelectedIndex = index;
        }

        private void moveUp_Click(object sender, EventArgs e)
        {
            int index = buildSeqList.SelectedIndex;
            if (index == 0)
            {
                return;
            }

            this.currentConfig.Steps[index].StepNumber--;
            for (int i = 0; i < this.currentConfig.Steps.Count; i++)
            {
                if (i != index)
                {
                    this.currentConfig.Steps[i].StepNumber++;
                }
            }

            index--;
            needsSave = true;
            IBuildStep selectedItem = (IBuildStep)buildSeqList.SelectedItem;
            buildSeqList.Items.Remove(selectedItem);
            buildSeqList.Items.Insert(index, selectedItem);
            buildSeqList.SelectedIndex = index;
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            if ((this.currentConfig != null && !this.currentConfig.Equals(ProjectService.BuildConfigs[currentIndex])) || needsSave)
                if (
                    MessageBox.Show(
                        "Would you like to save the current configuration?",
                        "Save?",
                        MessageBoxButtons.YesNo,
                        MessageBoxIcon.None) == DialogResult.Yes)
                {

                }
            {
            }

            {
            }

            {
            }

            {
            }

            {
            }

            {
            }

            {
            }

            {
                ProjectService.BuildConfigs[currentIndex] = currentConfig;
            }
        }

        private void outputBox_TextChanged(object sender, EventArgs e)
        {
            InternalBuildStep step = (InternalBuildStep)buildSeqList.SelectedItem;
            step.OutputFile = FileOperations.NormalizePath(Path.Combine(ProjectService.ProjectDirectory, outputBox.Text));
            needsSave = !isInternal;
        }

        private void PopulateListBox()
        {
            buildSeqList.Items.Clear();
            this.currentConfig.SortSteps();
            foreach (IBuildStep step in this.currentConfig.Steps)
            {
                buildSeqList.Items.Add(step);
            }

            buildSeqList.SelectedIndex = this.currentConfig.Steps.Count - 1;
        }

        private void stepTypeBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            IBuildStep step = null;
            int stepNum = ((IBuildStep)buildSeqList.SelectedItem).StepNumber;
            if (buildSeqList.SelectedItem is InternalBuildStep && stepTypeBox.SelectedIndex != 0)
            {
                step = new ExternalBuildStep(buildSeqList.SelectedIndex, "cmd.exe", string.Empty);
                step.StepNumber = stepNum;
            }
            else if (buildSeqList.SelectedItem is ExternalBuildStep && stepTypeBox.SelectedIndex != 1)
            {
                step = new InternalBuildStep(
                    buildSeqList.SelectedIndex,
                    BuildStepType.Assemble,
                    Path.ChangeExtension(ProjectService.ProjectFile, ".asm"),
                    Path.ChangeExtension(ProjectService.ProjectFile, ".8xk"));
                step.StepNumber = stepNum;
            }

            if (step != null)
            {
                this.currentConfig.Steps[buildSeqList.SelectedIndex] = step;
                buildSeqList.Items[buildSeqList.SelectedIndex] = step;
                this.UpdateStepOptions();
                needsSave = true;
            }
        }

        private void UpdateStepOptions()
        {
            if (buildSeqList.SelectedIndex == -1)
            {
                stepOptionsBox.Enabled = false;
                return;
            }
            else
            {
                stepOptionsBox.Enabled = true;
            }

            if (buildSeqList.SelectedItem is InternalBuildStep)
            {
                stepTypeBox.SelectedIndex = 0;
            }
            else if (buildSeqList.SelectedItem is ExternalBuildStep)
            {
                stepTypeBox.SelectedIndex = 1;
            }

            IBuildStep step = (IBuildStep)buildSeqList.SelectedItem;
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
    }
}