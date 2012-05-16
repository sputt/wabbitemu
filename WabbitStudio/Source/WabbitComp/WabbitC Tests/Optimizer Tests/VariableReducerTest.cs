using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using WabbitC.Model;
using WabbitC;
using WabbitC.Optimizer;

namespace WabbitC_Tests.Optimizer_Tests
{
    /// <summary>
    /// Summary description for VariableRducerTest
    /// </summary>
    [TestClass]
    public class VariableReducerTest
    {
        public VariableReducerTest()
        {
        }

        [TestMethod]
        public void VariableReducer_BasicReductionTest1()
        {
            string code = "int i = 0; int j = 5; int k = i; int l = j + k;";
            var tokens = Tokenizer.Tokenize(code);
            var tokenEnumerator = tokens.GetEnumerator();
            tokenEnumerator.MoveNext();
            var block = Block.ParseBlock(ref tokenEnumerator, new Module());
            VariableReducer.OptimizeBlock(ref block);
            //TODO: fix this so its not dependant on the code generated
            string expected = "{\r\nint i;\r\nint j;\r\nint k;\r\nint l;\r\nint __temp0;\r\n//i = 0;\r\ni = 0;\r\n//j = 5;\r\nj = 5;\r\n//k = i;\r\n" +
                    "k = i;\r\n//l = j + k;\r\n__temp0 = j;\r\n__temp0 += (int) i;\r\nl = __temp0;\r\n}\r\n";
            Assert.AreEqual(expected, block.ToString());
        }

        [TestMethod]
        public void VariableReducer_LoopReductionTest1()
        {
            string code = "int i = 0; int j = 5; while(1) { int k = i; int l = j + k; }";
            var tokens = Tokenizer.Tokenize(code);
            var tokenEnumerator = tokens.GetEnumerator();
            tokenEnumerator.MoveNext();
            var block = Block.ParseBlock(ref tokenEnumerator, new Module());
            VariableReducer.OptimizeBlock(ref block);
            //TODO: fix this so its not dependant on the code generated
            string expected = "{\r\nint i;\r\nint j;\r\nint __temp1;\r\n//i = 0;\r\ni = 0;\r\n//j = 5;\r\nj = 5;\r\n" +
                    "goto __label2;\r\ndo\r\n{\r\nint k;\r\nint l;\r\nint __temp0;\r\n//k = i;\r\nk = i;\r\n//l = j + k;" +
                    "\r\n__temp0 = j;\r\n__temp0 += (int) i;\r\nl = __temp0;\r\n__label2:\r\n;\r\n//__temp1 = 1;\r\n" + 
                    "__temp1 = 1;\r\n}\r\nwhile (__temp1);\r\n}\r\n";
            Assert.AreEqual(expected, block.ToString());
        }
    }
}
