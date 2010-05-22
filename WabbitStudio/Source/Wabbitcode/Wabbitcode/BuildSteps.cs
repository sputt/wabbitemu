using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Xml;
using Revsoft.Wabbitcode.Classes;

namespace Revsoft.Wabbitcode
{
    public partial class BuildSteps : Form
    {
        public string projectFile;
        public List<XmlElement> buildTargetsList = new List<XmlElement>();
        public List<XmlElement> fileContents = new List<XmlElement>();
        public BuildSteps(string projectFile)
        {
            InitializeComponent();
            this.projectFile = projectFile;
            doc = new XmlDocument();
            doc.Load(projectFile);
            getBuildTargets();
            populateListBox();
            configBox.SelectedIndex = 0;
            configBox.DropDownStyle = ComboBoxStyle.DropDownList;
        }

        readonly XmlDocument doc;
        private void getBuildTargets()
        {
            int old = configBox.SelectedIndex;
            configBox.Items.Clear();
            XmlNodeList buildTargets = doc.ChildNodes[1].ChildNodes[1].ChildNodes;
            foreach (XmlElement target in buildTargets)
            {
                buildTargetsList.Add(target);
                configBox.Items.Add(target.Name);
            }
            configBox.SelectedIndex = old;
        }

        private void populateListBox()
        {
            buildSeqList.Items.Clear();
            foreach (XmlElement line in fileContents)
            {
                //our compile statement has a num at the end we need to account for
                switch (line.Attributes["action"].Value)
                {
                    case "Compile":
                        buildSeqList.Items.Add("Assemble " + Path.GetFileName(line.InnerText));
                        break;
                    case "Table":
                        buildSeqList.Items.Add("Create symbol table for " + Path.GetFileName(line.InnerText));
                        break;
                    case "Listing":
                        buildSeqList.Items.Add("Build listing for " + Path.GetFileName(line.InnerText));
                        break;
                    case "External":
                        buildSeqList.Items.Add("Run " + Path.GetFileName(line.InnerText));
                        break;
                }
            }
        }

        private void addDirButton_Click(object sender, EventArgs e)
        {
            if (buildSeqList.SelectedIndex == -1 && buildSeqList.Items.Count > 0)
                buildSeqList.SelectedIndex = 0;
            BuildForm newBuildStep = new BuildForm(projectFile) 
            {actionBox = {SelectedIndex = 0}};
            if (newBuildStep.ShowDialog() != DialogResult.OK) 
                return;
            if (buildSeqList.SelectedIndex == -1)
                fileContents.Add(newBuildStep.buildStep);
            else
                fileContents.Insert(buildSeqList.SelectedIndex, newBuildStep.buildStep);
            populateListBox();
        }

        private void deleteDirButton_Click(object sender, EventArgs e)
        {
            object index = buildSeqList.SelectedItem;
            if (index == null)
                return;
            //string line = index.ToString();
            //switch (line[0])
            //{
            //    case 'A':
            //        line = line.Substring(9, line.Length - 9);
            //        break;
            //    case 'C':
            //        line = line.Substring(24, line.Length - 24);
            //        break;
            //    case 'B':
            //        line = line.Substring(18);
            //        break;
            //    case 'R':
            //        line = line.Substring(4, line.Length - 4);
            //        break;
            //}
            XmlElement lineTxt = fileContents[buildSeqList.SelectedIndex];
            fileContents.Remove(lineTxt);
            buildSeqList.Items.Remove(index);
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            try
            {
                saveBuildConfig();
                doc.Save(projectFile);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: " + ex);
            }            
        }

        private void editButton_Click(object sender, EventArgs e)
        {
            object index = buildSeqList.SelectedItem;
            int selectedIndex = buildSeqList.SelectedIndex;
            if (index == null) 
                return;
            BuildForm editStep = new BuildForm(projectFile);
            switch (index.ToString()[0])
            {
                case 'A':
                    editStep.actionBox.SelectedIndex = 0;
                    int type = Convert.ToInt16(fileContents[selectedIndex].Attributes["type"].Value);
                    editStep.outputBox.SelectedIndex = type;
                    editStep.fileLocBox.Text = fileContents[selectedIndex].InnerText;
                    break;
                case 'C':
                    editStep.actionBox.SelectedIndex = 1;
                    editStep.fileLocBox.Text = fileContents[selectedIndex].InnerText;
                    break;
                case 'B':
                    editStep.actionBox.SelectedIndex = 2;
                    editStep.fileLocBox.Text = fileContents[selectedIndex].InnerText;
                    break;
                case 'R':
                    editStep.actionBox.SelectedIndex = 3;
                    editStep.commandBox.Text = fileContents[selectedIndex].InnerText;
                    break;
            }
            if (editStep.ShowDialog() != DialogResult.OK) 
                return;
            //why didnt i think of this the first time dammit!?
            fileContents.Remove(fileContents[selectedIndex]);//"B " + fileContents[selectedIndex].Substring(2, 1) + " " + index.ToString().Substring(4, fileContents[selectedIndex].Length - 4));
            fileContents.Insert(selectedIndex, editStep.buildStep);
            populateListBox();
        }

        private void moveUp_Click(object sender, EventArgs e)
        {
            int index = buildSeqList.SelectedIndex;
            if (index < 1)
                return;
            //string stepText = buildSeqList.SelectedItem.ToString();
            XmlElement stepLine = fileContents[index];
            fileContents.Remove(stepLine);
            fileContents.Insert(index - 1, stepLine);
            populateListBox();
            buildSeqList.SelectedIndex = index;
            buildSeqList.SelectedIndex--;
        }

        private void moveDown_Click(object sender, EventArgs e)
        {
            int index = buildSeqList.SelectedIndex;
            if (index + 1 > buildSeqList.Items.Count - 1)
                return;
            //string stepText = buildSeqList.SelectedItem.ToString();
            XmlElement stepLine = fileContents[index];
            fileContents.Insert(index + 2, stepLine);
            fileContents.Remove(stepLine);
            populateListBox();
            buildSeqList.SelectedIndex = index;
            buildSeqList.SelectedIndex++;
        }

        private void configBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            fileContents.Clear();
            XmlNodeList list = buildTargetsList[configBox.SelectedIndex].ChildNodes;
            foreach (XmlElement element in list)
                fileContents.Add(element);
            populateListBox();
        }

        private void saveBuildConfig()
        {
            doc.ChildNodes[1].ChildNodes[1].ChildNodes[configBox.SelectedIndex].RemoveAll();
            foreach (XmlElement element in fileContents)
                doc.ChildNodes[1].ChildNodes[1].ChildNodes[configBox.SelectedIndex].AppendChild(doc.ImportNode(element, true));
            getBuildTargets();
        }
        /// <summary>
        /// This is a hack to remember the config so we can save it
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void configBox_DropDown(object sender, EventArgs e)
        {
            saveBuildConfig();
        }
    }
}