using Microsoft.VisualStudio.TestTools.UnitTesting;
using WabbitC;
using WabbitC.Optimizer;

namespace WabbitC_Tests
{
    [TestClass]
    public class LiveAnalysisTest
    {
        [TestMethod]
        public void GenTest1()
        {
            /*var module = Compiler.DoCompile("int main() { a = 3; b = 5; d = 4; x = 100; if (a > b) { c = a + b; d = 2; } c = 4; return b * d + c; }", Compiler.OptimizeLevel.OptimizeNone, Compiler.PassCount.Pass1);
            VariableReuseClass.Gen(block);*/
        }
    }
}
