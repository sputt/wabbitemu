using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using WabbitC;
using System.IO;

namespace WabbitC_Tests
{
    [TestClass]
    public class IntermediateCompileTest
    {
        private void RunIntermediateTest(string name)
        {
            string CurDir = Environment.CurrentDirectory;
            var compiler = new Compiler();
            Compiler.DoCompile(CurDir + @"\..\..\..\WabbitC Tests\C Files\" + name + "_expected.c");

            System.Diagnostics.Process p = new System.Diagnostics.Process();
            p.StartInfo.WorkingDirectory = CurDir + @"\..\..\..\WabbitC Tests";
            p.StartInfo.FileName = p.StartInfo.WorkingDirectory + @"\build-intermediates.bat";
            p.StartInfo.Arguments = name;
            p.StartInfo.UseShellExecute = true;
            p.StartInfo.CreateNoWindow = true;
            p.StartInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;

            p.Start();
            p.WaitForExit();

            p.StartInfo.FileName = Path.Combine(p.StartInfo.WorkingDirectory, name + "_expected.exe");
            p.StartInfo.Arguments = "7";
            p.Start();
            p.WaitForExit();

            int ExpectedValue = p.ExitCode;

            System.IO.File.Delete(p.StartInfo.FileName);

            p.StartInfo.FileName = Path.Combine(p.StartInfo.WorkingDirectory, name + "_actual.exe");
            p.StartInfo.Arguments = "7";
            p.Start();
            p.WaitForExit();

            int ActualValue = p.ExitCode;

            System.IO.File.Delete(p.StartInfo.FileName);

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

        [TestMethod]
        public void Fibonacci()
        {
            RunIntermediateTest("fibonacci");
        }
    }
}
