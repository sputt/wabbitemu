using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using WabbitC;
using System.IO;
using System.Diagnostics;

namespace WabbitC_Tests
{
    [TestClass]
    public class IntermediateCompileTest
    {
        private TestContext testContextInstance;

        /// <summary>
        ///Gets or sets the test context which provides
        ///information about and functionality for the current test run.
        ///</summary>
        public TestContext TestContext
        {
            get
            {
                return testContextInstance;
            }
            set
            {
                testContextInstance = value;
            }
        }

        private void RunIntermediateTest(string name)
        {
            string args = "";
            int passCount = int.MaxValue;
            if (testContextInstance.DataRow != null)
            {
                if (Boolean.Parse(testContextInstance.DataRow["Enabled"].ToString()) == false)
                {
                    Assert.IsTrue(true);
                    Debug.WriteLine("Test was skipped because this mode is disabled.");
                    return;
                }

                args = testContextInstance.DataRow["Args"].ToString();
                passCount = int.Parse(testContextInstance.DataRow["PassCount"].ToString());
                if (passCount == -1)
                {
                    passCount = int.MaxValue;
                }
                Debug.WriteLine("Running config " + testContextInstance.DataRow["Name"] + ": " + args);
            }

            string CurDir = Environment.CurrentDirectory;
            var compiler = new Compiler();

            Compiler.OptimizeLevel opLevel = WabbitC.Optimizer.Optimizer.ParseCommandLine(args);
			Compiler.DoCompile(CurDir + @"\..\..\..\WabbitC Tests\C Files\" + name + "_expected.c", opLevel, passCount);

            System.Diagnostics.Process p = new System.Diagnostics.Process();
            p.StartInfo.WorkingDirectory = CurDir + @"\..\..\..\WabbitC Tests";
            p.StartInfo.FileName = p.StartInfo.WorkingDirectory + @"\build-intermediates.bat";
            p.StartInfo.Arguments = name;
            p.StartInfo.UseShellExecute = true;
            p.StartInfo.CreateNoWindow = true;
            p.StartInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;

            p.Start();
            p.WaitForExit();

			
            Assert.AreEqual(p.ExitCode, 0, "Failed to build");


            p.StartInfo.FileName = Path.Combine(p.StartInfo.WorkingDirectory, name + "_expected.exe");
            p.StartInfo.Arguments = "7";
            p.Start();
            p.WaitForExit();

            int ExpectedValue = p.ExitCode;

            p.StartInfo.FileName = Path.Combine(p.StartInfo.WorkingDirectory, name + "_actual.exe");
            p.StartInfo.Arguments = "7";
            p.Start();
            p.WaitForExit();

            int ActualValue = p.ExitCode;

            Assert.AreEqual(ExpectedValue, ActualValue);
        }

        [TestMethod]
        public void Return()
        {
            RunIntermediateTest("returntest");
        }

        [TestMethod]
        public void FunctionCallMath()
        {
            RunIntermediateTest("functioncallmath");
        }

        [TestMethod]
        public void SimpleMath()
        {
            RunIntermediateTest("SimpleMath");
        }


        [DataSource("Microsoft.VisualStudio.TestTools.DataSource.CSV", 
            "|DataDirectory|\\IntermediateCompileTestArgs.csv", 
            "IntermediateCompileTestArgs#csv",
            DataAccessMethod.Sequential), 
        DeploymentItem("WabbitC Tests\\IntermediateCompileTestArgs.csv"),
        TestMethod]
        public void Fibonacci()
        {
            RunIntermediateTest("fibonacci");
        }

        [TestMethod]
        public void Loop()
        {
            RunIntermediateTest("loop");
        }

        [TestMethod]
        public void Store()
        {
            RunIntermediateTest("store");
        }

        [TestMethod]
        public void Cast1()
        {
            RunIntermediateTest("cast");
        }

        public void BubbleSort()
		{
			RunIntermediateTest("bubblesort");
		}
    }
}
