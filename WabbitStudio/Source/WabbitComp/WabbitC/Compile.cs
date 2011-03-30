using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Diagnostics;
using WabbitC.Model;
using WabbitC.TokenPasses;
using WabbitC.Optimizer;

namespace WabbitC
{
    public class Compiler
    {
        private static List<Token> RunTokenPasses(List<Token> tokens)
        {
            List<Token> resultTokens = tokens;
            var asm = System.Reflection.Assembly.GetExecutingAssembly();
            foreach (System.Type type in asm.GetTypes())
            {
                if (type.Namespace != null && type.Namespace.Contains("TokenPasses"))
                {
                    if (type.Name != "TokenPass")
                    {
                        var pass = asm.CreateInstance(type.FullName) as TokenPass;
                        resultTokens = pass.Run(resultTokens);
                    }
                }
            }
            return resultTokens;
        }

		public enum OptimizeLevel
		{
			OptimizeNone,
			OptimizeSome,
			OptimizeAlot,
			OptimizeMax,
		}

        static OptimizeLevel optimizeLevel = OptimizeLevel.OptimizeMax;

        /// <summary>
        /// Parses a file and outputs a .asm file
        /// </summary>
        /// <returns></returns>
        public static bool DoCompile(string inputFile, OptimizeLevel opLevel = OptimizeLevel.OptimizeNone, int passCount = int.MaxValue)
        {
			optimizeLevel = opLevel;
            string fileContents = TryOpenFile(inputFile);
            if (string.IsNullOrEmpty(fileContents))
                return false;

            var tokenizerTokens = Tokenizer.Tokenize(fileContents);

			var preprocessorParser = new PreprocessorParser(tokenizerTokens);
			var preProcessorTokens = preprocessorParser.Parse();

            var tokenPassTokens = RunTokenPasses(preProcessorTokens);

            var tokens = tokenPassTokens.GetEnumerator();
            tokens.MoveNext();

            var currentModule = Module.ParseModule(ref tokens);

            // Statement passes
            if (passCount >= 2)
            {
                StatementPasses.BlockCollapse.Run(currentModule);
                StatementPasses.IfGotoConversion.Run(currentModule);
                StatementPasses.LoopGotoConversion.Run(currentModule);
            }

			if (optimizeLevel != OptimizeLevel.OptimizeNone)
				Optimizer.Optimizer.RunOptimizer(ref currentModule, optimizeLevel);

            if (passCount >= 3)
            {
                StatementPasses.StackAllocator.Run(currentModule);
                StatementPasses.AddStackDeclaration.Run(currentModule);

                //if (optimizeLevel != OptimizeLevel.OptimizeNone)
                //    StatementPasses.SmarterRegisterAllocator.Run(currentModule);
                //else
                //    StatementPasses.DumbRegisterAllocator.Run(currentModule);

                //AssemblyGenerator codeGenerator = new AssemblyGenerator(currentModule);
                //codeGenerator.GenerateCode();
            }

            string code = currentModule.ToString();

            string outputFile = inputFile.Replace("expected", "actual");
            if (outputFile.Equals(inputFile))
            {
                outputFile = Path.GetFileNameWithoutExtension(inputFile) + "_compiled.c";
            }

            var writer = new StreamWriter(outputFile);
            writer.Write(code.Replace("\n", "\r\n"));
            writer.Close();

            return true;
        }

        public static string TryOpenFile(string inputFile)
        {
            string fileContents;
            try
            {
                fileContents = new StreamReader(inputFile).ReadToEnd();
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.ToString());
                return null;
            }
            return fileContents;
        }
    }
}
