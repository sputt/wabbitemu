using System;
using System.Collections;
using System.IO;
using System.Windows.Forms;
using System.Xml;
using Revsoft.Wabbitcode.Properties;
using Revsoft.Wabbitcode.Services;

namespace Revsoft.Wabbitcode.Classes
{
    public class ProjectClass
    {
        /// <summary>
        /// This is the path to the project folder
        /// </summary>
        public string projectLoc = "";
        
        /// <summary>
        /// The name of the project.
        /// </summary>
        public string projectName {
			get {
				 if (doc == null)
	                loadDocument();
				 if (doc.ChildNodes.Count < 1)
					 return "";
	            string name = Path.GetFileNameWithoutExtension(doc.ChildNodes[1].Attributes["name"].Value);
	            return name;
			}
			set {
				if (doc == null)
					loadDocument();
				doc.ChildNodes[1].Attributes["name"].Value = Path.ChangeExtension(value, ".wcodeproj")	;
				saveProject();
			}
		}
        
        /// <summary>
        /// Arraylist of all the labels in the project.
        /// </summary>
        public ArrayList projectLabels = new ArrayList();

        /// <summary>
        /// Specifies whether a project is currently open
        /// </summary>
        public bool projectOpen;

        /// <summary>
        /// This is the path to the actual .wcodeproj file
        /// </summary>
        public string wcodeProjectFile = "";
        public bool isInternal;

        public ProjectClass( string projectFile)
        {
            if (string.IsNullOrEmpty(projectFile))
            {
                isInternal = true;
                return;
            }
            projectLabels = new ArrayList();
            //this will get changed in build project tree
            //why dont i change it here? idk why ...see it was a bad idea, now i do change it here
            wcodeProjectFile = projectFile;
            projectLoc = Path.GetDirectoryName(wcodeProjectFile);
            projectOpen = true;
            Settings.Default.includeDir = "";
			//projectName = ProjectService.ProjectName;
            //change the ribbon output box to reflect a project
            //assemblerOutputBox.Visible = false;
            //buildSeqButton.Visible = true;

            if (Settings.Default.startupProject != wcodeProjectFile)
                if (MessageBox.Show("Would you like to make this your default project?", "Startup Project",
                                    MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                    Settings.Default.startupProject = wcodeProjectFile;
        }

       

        public void CloseProject()
        {
            projectOpen = false;
            isInternal = true;
			if (doc == null)
				loadDocument();
            DockingService.ProjectViewer.buildXMLFile(ref doc);
            saveProject();
        }

        /// <summary>
        /// Gets all files in the current project. DO NOT CALL THIS UNTIL YOU HAVE GOT ALL YOUR INCLUDE DIRS!
        /// </summary>
        public ArrayList getAllProjectFiles()
        {
            ArrayList fileList = new ArrayList();
            if (isInternal)
            {
                foreach (string dir in Settings.Default.includeDir.Split('\n'))
                {
                    if (dir == "")
                        continue;
                    string[] files = Directory.GetFiles(dir);
                    foreach (string file in files)
                        if (file.EndsWith(".asm") || file.EndsWith(".z80") || file.EndsWith(".inc"))
                            fileList.Add(file);
                }
                string[] projFiles = Directory.GetFiles(projectLoc);
                foreach (string file in projFiles)
                    if (file.EndsWith(".asm") || file.EndsWith(".z80") || file.EndsWith(".inc"))
                        fileList.Add(file);
            }
            else
                fileList = getProjFilesFullPaths();
            return fileList;
        }

        public void getAllProjectLabels(ArrayList files)
        {
			string file = "";
			try
			{
				FileStream stream;
				StreamReader reader;
				//int counter = 0;
				for(int i = 0; i < files.Count; i++)
				{
					file = files[i].ToString();
					if (!File.Exists(file))
						continue;
					stream = new FileStream(file, FileMode.Open);
					reader = new StreamReader(stream);
					string[] lines = reader.ReadToEnd().Split('\n');
					ArrayList temp = getAllLabels(lines, true, file);
					//if (((ArrayList)temp[1]).Count != 0)
					projectLabels.Add(temp);
					//counter++;
					stream.Flush();
					stream.Close();
				}
			}
			catch (Exception ex)
			{
				MessageBox.Show("Error getting project labesls from file: " + file + "\n" + ex.ToString());
			}
        }

        public ArrayList getAllLabels(string[] lines, bool includeEquates, string fileName)
        {
            ArrayList includeFile = new ArrayList {new ArrayList(), new ArrayList(), new ArrayList()};
            ArrayList labels = (ArrayList)includeFile[0];
            ArrayList properties = (ArrayList)includeFile[1];
            ArrayList description = (ArrayList)includeFile[2];
            int location = 0;
            int counter = 0;
            string text;
            string endText = "|label|";
            foreach (string line in lines)
            {
                int lineLength = line.Length;
                if (lineLength <= 0) 
                    continue;
                char firstChar = line[0];
                if (char.IsWhiteSpace(line[0]) || "\r \t;.".IndexOf(firstChar) != -1 && (!line.Contains("#define") || !line.Contains("#macro")))
                {
                    location += lineLength + 1;
                    counter++;
                    continue;
                }
                //Find first word
                char[] whiteSpaceNext = { ' ', '\t', '\n', ':', '\r' };
                int select = line.IndexOfAny(whiteSpaceNext);
                //Check if this is a label or an equate
                int label = line.IndexOf(':');
                //Check make sure the first char is not whitespace, check that its either a label or an equate (if we need to find them)
                if (label == select && label != -1 || includeEquates && ((select != -1 && line[0] != '#') || line.StartsWith("#define") || line.StartsWith("#macro")))
                {
                    //if were including equates and theres no colon, its an equate, lets set it to that
                    if (includeEquates && label == -1)
                    {
                        if (line.Contains("=") || line.ToLower().Contains("equ") || line.ToLower().Contains("#define") || line.ToLower().Contains("#macro"))
                        {
                            label = select;
                            endText = "|equate|";
                        }
                        else
                        {
                            location += lineLength + 1;
                            counter++;
                            continue;
                        }
                    }
                    if (label == -1)
                        label = 1;
                    //if its case sensitive we need to put it into lowercse
                    if (line.Contains("#define") || line.Contains("#macro"))
                    {
                        label = line.IndexOf("#define") + 8;
                        if (label == 7)
                            label = line.IndexOf("#macro") + 7;
                        if (label > line.Length || line.IndexOfAny(whiteSpaceNext, label) == -1)
                        {
                            location += lineLength + 1;
                            counter++;
                            continue;
                        }
                        char[] defineSpace = { ' ', '\t', '\n', ':', '\r', '(' };
                        text = line.Substring(label, line.IndexOfAny(defineSpace, label) - label);
                        if ((line.IndexOf('(', label) != -1 && line.Contains("#define") || line.Contains("#macro")))
                            endText = "|macro|";                        
                    }
                    else
                        text = line.Substring(0, label);
                    if (Settings.Default.caseSensitive)
                        labels.Add(text);
                    else
                        labels.Add(text.ToLower());
                    properties.Add(fileName + endText + location);
                    int back = 1;
                    text = "";
                    if (counter - back > 0)
                    {
                        while (lines[counter - back].StartsWith(";"))
                        {
                            text = lines[counter - back] + text;
                            back++;
                            if (counter - back < 0)
                                break;
                        }
                    }
                    description.Add(text.Replace('\r', '\n'));
                    //reset the ending
                    endText = "|label|";
                }
                location += lineLength + 1;
                counter++;
            }
            return includeFile;
        }

        #region XMLStuffs

        public string[] getIncludeDirs()
        {
            if (doc == null)
                loadDocument();
            XmlNode includeNode = doc.ChildNodes[1].ChildNodes[2];
            if (includeNode == null)
            {
                if (doc.ChildNodes[1].ChildNodes.Count < 3)
                {
                    XmlAttribute temp = doc.CreateAttribute("count");
                    temp.Value = "0";
                    XmlElement node = doc.CreateElement("Includes");
                    node.Attributes.Append(temp);
                    doc.ChildNodes[1].AppendChild(node);
                }
                return new string[1];
            }
            if (includeNode.Attributes["count"] == null)
                return new string[1];
            string[] dirs = new string[Convert.ToInt16(includeNode.Attributes["count"].Value)];
            int counter = 0;
            foreach(XmlNode node in includeNode.ChildNodes)
            {
                dirs[counter] = node.InnerText;
                counter++;
            }
            return dirs;
        }

        public void setIncludeDirs(string[] dirs)
        {
            if (doc == null)
                loadDocument();
            if (doc.ChildNodes[1].ChildNodes.Count < 3)
                doc.ChildNodes[1].AppendChild(doc.CreateElement("Includes"));
            XmlNode includeNode = doc.ChildNodes[1].ChildNodes[2];
            includeNode.RemoveAll();
            int counter = 0;
            foreach (string dir in dirs)
            {
                if (string.IsNullOrEmpty(dir))
                    continue;
                XmlNode node = doc.CreateElement("dir");
                node.InnerText = dir;
                includeNode.AppendChild(node);
                counter++;
            }
            if (includeNode.Attributes["count"] == null)
            {
                XmlAttribute temp = doc.CreateAttribute("count");
                includeNode.Attributes.Append(temp);
            }
            includeNode.Attributes["count"].Value = counter.ToString();
            saveProject();

    }

        public XmlNodeList getBuildConfigs()
        {
            loadDocument();
            return doc.ChildNodes[1].ChildNodes[1].ChildNodes;
            //ArrayList buildConfigs = new ArrayList();
            //foreach (XmlElement element in doc.ChildNodes[1].ChildNodes[1].ChildNodes)
                //buildConfigs.Add(element.Value);
            //return buildConfigs;
        }


		//public ArrayList getProjDirs()
		//{
		//    ArrayList directories = new ArrayList();
		//    FileStream stream = new FileStream(wcodeProjectFile, FileMode.Open);
		//    XmlTextReader reader = new XmlTextReader(stream);
		//    while (reader.Read())
		//        if (reader.NodeType == XmlNodeType.Element && reader.Name == "dir")
		//            directories.Add(reader["name"]);
		//    stream.Flush();
		//    stream.Close();
		//    return directories;
		//}

		//public ArrayList getProjFiles()
		//{
		//    ArrayList files = new ArrayList();
		//    FileStream stream = new FileStream(wcodeProjectFile, FileMode.Open);
		//    XmlTextReader reader = new XmlTextReader(stream);
		//    string dir = "";
		//    while (reader.Read())
		//    {
		//        if (reader.NodeType == XmlNodeType.Element)
		//        {
		//            if (reader.Name == "dir")
		//                dir += reader["name"];
		//            if (reader.Name == "file")
		//                files.Add(dir + reader.ReadString());
		//        }
		//        if ((reader.NodeType == XmlNodeType.EndElement || reader.IsEmptyElement) && reader.Name == "dir")
		//            dir = dir.LastIndexOf('\\') != dir.IndexOf('\\') ? dir.Substring(0, dir.LastIndexOf('\\')) : "";
		//    }
		//    stream.Flush();
		//    stream.Close();
		//    return files;
		//}

        public ArrayList getProjFilesFullPaths()
        {
			ArrayList files = new ArrayList();
			try
			{
				FileStream stream = new FileStream(wcodeProjectFile, FileMode.Open);
				XmlTextReader reader = new XmlTextReader(stream);
				string dir = "";
				while (reader.Read())
				{
					if (reader.NodeType == XmlNodeType.Element)
					{
						if (reader.Name == "dir")
							dir += reader["name"];
						if (reader.Name == "file")
							files.Add(Path.GetDirectoryName(wcodeProjectFile) + reader["Path"]);
					}
					if ((reader.NodeType == XmlNodeType.EndElement || reader.IsEmptyElement) && reader.Name == "dir")
						dir = dir.LastIndexOf('\\') != dir.IndexOf('\\') ? dir.Substring(0, dir.LastIndexOf('\\')) : "";
				}
				stream.Flush();
				stream.Close();
			}
			catch (Exception ex)
			{
				MessageBox.Show("Error reading the project file!\n" + ex);
			}
            return files;
        }

        public void addDir(string dir, string parentDir, bool addchildren)
        {
            if (doc == null)
                loadDocument();
            //XmlNodeList dirList = doc.GetElementsByTagName("dir");
            XmlNode dirToAddFile = FindFolder(doc.DocumentElement.ChildNodes[0].ChildNodes, parentDir);
            if (dirToAddFile == null)
                throw new DirectoryNotFoundException();
            XmlElement newDir = doc.CreateElement("dir");
            XmlAttribute dirNameAtr = doc.CreateAttribute("name");
            dirNameAtr.Value = dir;
            newDir.Attributes.Append(dirNameAtr);
            try
            {
                if (addchildren && !string.IsNullOrEmpty(innerXMLSaved))
                    newDir.InnerXml = innerXMLSaved;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            dirToAddFile.AppendChild(newDir);
        }

        private XmlDocument doc;
        private void loadDocument()
        {
            doc = new XmlDocument();
            doc.Load(wcodeProjectFile);
        }
        
        public bool addFile(string file, string dir, string fullPath)
        {
            if (doc == null)
                loadDocument();
            XmlNode dirToAddFile = FindFolder(doc.DocumentElement.ChildNodes[0].ChildNodes, dir);
            if (dirToAddFile == null)
                throw new DirectoryNotFoundException();
            //return false;
            XmlNode newFile = doc.CreateElement("file");
            newFile.InnerText = file;
            XmlAttribute test = doc.CreateAttribute("Path");
            test.Value = fullPath;
            newFile.Attributes.Append(test);
            ArrayList files = getProjFilesFullPaths();
            int length = Path.GetDirectoryName(wcodeProjectFile).Length;
            foreach (string node in files)
                if (node.Remove(0, length) == fullPath)
                    return false;
            dirToAddFile.AppendChild(newFile);
            return true;
        }

        public void saveProject()
        {
            if (doc == null)
                loadDocument();
            doc.Save(wcodeProjectFile);
        }

        private string currentDir = "";
        XmlElement elementToReturn;
        private XmlElement FindFolder(XmlNodeList children, string dir)
        {
			if (dir.StartsWith(ProjectService.ProjectName))
            {
                currentDir = "";
                elementToReturn = null;
                dir = dir.Remove(0, ProjectService.ProjectName.Length + 1);
            }
            if (dir == "")
                return (XmlElement)doc.DocumentElement.ChildNodes[0];
            foreach (XmlElement dirName in children)
            {
                if (dirName.Name != "dir") 
                    continue;
                currentDir += dirName.Attributes["name"].Value;
                if (currentDir == dir)
                {
                    elementToReturn = dirName;
                    break;
                }
                elementToReturn = FindFolder(dirName.ChildNodes, dir);
                if (elementToReturn == null)
                {
                    currentDir = currentDir.LastIndexOf('\\', currentDir.Length - 2) == -1 ? "" : currentDir.Substring(0, currentDir.LastIndexOf('\\', currentDir.Length - 2) + 1);
                }
            }
            return elementToReturn;
        }

        public void deleteFile(string file, string dir)
        {
            if (doc == null)
                loadDocument();
            XmlNode dirToDelFile = FindFolder(doc.DocumentElement.ChildNodes[0].ChildNodes, dir);
            if (dirToDelFile == null)
                throw new DirectoryNotFoundException();
            XmlNode fileToDel = null;
            XmlNodeList newFile = dirToDelFile.ChildNodes;
            foreach (XmlNode fileNode in newFile)
                if (fileNode.InnerText == file)
                {
                    fileToDel = fileNode;
                    break;
                }
            if (fileToDel == null)
                throw new FileNotFoundException();
            dirToDelFile.RemoveChild(fileToDel);
        }

        public void deleteFolder(string parentdir, string dir)
        {
            if (doc == null)
                loadDocument();
            XmlNode dirToDelFile = FindFolder(doc.DocumentElement.ChildNodes[0].ChildNodes, parentdir);
            if (dirToDelFile == null)
                throw new DirectoryNotFoundException();
            XmlElement dirToDel = null;
            foreach (XmlElement possibledirs in dirToDelFile)
                if (possibledirs.Name == "dir" && possibledirs.Attributes["name"].Value == dir)
                {
                    dirToDel = possibledirs;
                    break;
                }
            if (dirToDel == null)
                throw new DirectoryNotFoundException();
            innerXMLSaved = dirToDel.InnerXml;
            dirToDelFile.RemoveChild(dirToDel);
        }

        private string innerXMLSaved;
        public void addFoldingAttribute(string file, string dir, string foldingSerialized)
        {
            if (doc == null)
                loadDocument();
            XmlNode parentDir = FindFolder(doc.DocumentElement.ChildNodes[0].ChildNodes, dir);
            if (parentDir == null)                          //this should only happen if we get a bad path
                throw new DirectoryNotFoundException();
            XmlNode fileToFind = null;
            XmlNodeList newFile = parentDir.ChildNodes;
            foreach (XmlNode fileNode in newFile)
                if (fileNode.InnerText == file)
                {
                    fileToFind = fileNode;
                    break;
                }
            if (fileToFind == null)
                throw new FileNotFoundException();
            XmlAttribute foldings = doc.CreateAttribute("Foldings");
            foldings.Value = foldingSerialized;
            fileToFind.Attributes.Append(foldings);
        }

        public string getProjectFileFullPath(string file, string dir)
        {
            if (doc == null)
                loadDocument();
            XmlNode parentDir = FindFolder(doc.DocumentElement.ChildNodes[0].ChildNodes, dir);
            if (parentDir == null)                          //this should only happen if we get a bad path
                throw new DirectoryNotFoundException();
            XmlNode fileToFind = null;
            XmlNodeList newFile = parentDir.ChildNodes;
            foreach (XmlNode fileNode in newFile)
                if (fileNode.InnerText == file)
                {
                    fileToFind = fileNode;
                    break;
                }
            if (fileToFind == null)
                throw new FileNotFoundException();
            fileAttributes = fileToFind.Attributes;
            return fileToFind.Attributes["Path"].Value;
        }

        public XmlAttributeCollection fileAttributes;
        public void renameFile(NodeLabelEditEventArgs e)
        {
            if (e.Node.Tag.ToString() == "Folder")
            {
                //string projectLoc = Path.GetDirectoryName(projectFile); //projectFile.Substring(0, projectFile.LastIndexOf('\\'));
                string dir = e.Node.FullPath.Substring(e.Node.FullPath.LastIndexOf('\\') + 1, e.Node.FullPath.Length - e.Node.FullPath.LastIndexOf('\\') - 1) + "\\";
                string parentdir = e.Node.FullPath.Substring(0, e.Node.FullPath.LastIndexOf('\\') + 1);
                //if (parentdir == projectFile.Substring(projectFile.LastIndexOf('\\') + 1, projectFile.LastIndexOf('.') - projectFile.LastIndexOf('\\') - 1) + "\\")
                //    parentdir = "";
                deleteFolder(parentdir, dir);
                addDir(e.Label + "\\", parentdir, true);
            }
            else
            {
                //string projectLoc = Path.GetDirectoryName(wcodeProjectFile); //projectFile.Substring(0, projectFile.LastIndexOf('\\'));
                string fileName = e.Node.FullPath.Substring(e.Node.FullPath.LastIndexOf('\\') + 1, e.Node.FullPath.Length - e.Node.FullPath.LastIndexOf('\\') - 1);
                string dir = e.Node.FullPath.Substring(0, e.Node.FullPath.LastIndexOf('\\') + 1);
                //if (dir == projectFile.Substring(projectFile.LastIndexOf('\\') + 1, projectFile.LastIndexOf('.') - projectFile.LastIndexOf('\\') - 1) + "\\")
                //    dir = "";
                string file = getProjectFileFullPath(fileName, dir);
                file = file.Substring(0, file.LastIndexOf('\\') + 1) + e.Label;
                File.Move(projectLoc.Substring(0, projectLoc.LastIndexOf('\\') + 1) + e.Node.FullPath, projectLoc + '\\' + file);
                deleteFile(fileName, dir);
                addFile(e.Label, dir, file);
            }
            saveProject();
        }
        /*public void renameFolder(string projectFile, string oldFolderName, string newFolderName)
        {
            ArrayList folders = getProjDirs();
            if (!folders.Contains(oldFolderName))
                throw new DirectoryNotFoundException();
        }*/

        public int getOutputType()
        {
            if (doc == null)
                loadDocument();
            XmlNodeList children = doc.ChildNodes[1].ChildNodes[1].ChildNodes[Settings.Default.buildConfig].ChildNodes;
            foreach (XmlElement child in children)
                if (child.Attributes["action"].Value == "Compile")
                    return Convert.ToInt32(child.Attributes["type"].Value);
            return -1;
        }
        #endregion
    }
}
