using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using WabbitC.StatementPasses;
using WabbitC.Model;
using WabbitC;

namespace WabbitC_Tests.StatementPass_Tests
{
    [TestClass]
    public class ReplaceLocalsWithGlobalsTest
    {
        [TestMethod]
        public void ReplaceLocalsWithGlobals_Test1()
        {
            var tokens = Tokenizer.Tokenize("void main() { int i; int j; i = 0; j = 1; } void add() { int i; int j; i = 0; j = 1; }");
            var tokensList = tokens.GetEnumerator();
            tokensList.MoveNext();
            Module module = Module.ParseModule(ref tokensList);
            ReplaceLocalsWithGlobals.Run(module);
            Assert.IsTrue(module.GlobalVariables.Count == 2);
        }

        [TestMethod]
        public void ReplaceLocalsWithGlobals_Test2()
        {
            var tokens = Tokenizer.Tokenize("void add() { int i; int j; i = 0; j = 1; } void main() { int i; int j; i = 0; j = 1; add(); }");
            var tokensList = tokens.GetEnumerator();
            tokensList.MoveNext();
            Module module = Module.ParseModule(ref tokensList);
            ReplaceLocalsWithGlobals.Run(module);
            Assert.IsTrue(module.GlobalVariables.Count == 4);
        }
    }
}
