using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Diagnostics;
using WabbitC.Model;
using WabbitC.TokenPasses;

namespace WabbitC
{
    public class Compiler
    {
        /// <summary>
        /// Parses a file and outputs a .asm file
        /// </summary>
        /// <returns></returns>
        public static bool DoCompile(string inputFile)
        {
            string fileContents = TryOpenFile(inputFile);
            if (string.IsNullOrEmpty(fileContents))
                return false;

            var tokenizer = new Tokenizer();
			tokenizer.Tokenize(fileContents);//"((test2 + 10) + 40) + 20");
			/*Expression exp = new Expression(tokenizer.Tokens);
			var test = exp.Eval();*/

			var preprocessorParser = new PreprocessorParser(tokenizer.Tokens);
			var preProcessorTokens = preprocessorParser.Parse();

            var tokenPass1 = new ArrayDereference();
            var newTokens = tokenPass1.Run(preProcessorTokens);

            var tokenPass2 = new Bracer();
            newTokens = tokenPass2.Run(newTokens);

			var tokenPass3 = new ForLoopRemover();
			newTokens = tokenPass3.Run(newTokens);

			var tokenPass4 = new CompoundAssignmentRemover();
			newTokens = tokenPass4.Run(newTokens);

            var tokens = newTokens.GetEnumerator();
            tokens.MoveNext();

            Module currentModule = Module.ParseModule(ref tokens);

			AssemblyGenerator codeGenerator = new AssemblyGenerator(currentModule);
			codeGenerator.GenerateCode();

            string code = currentModule.ToString();

            var writer = new StreamWriter("test_compiled.c");
            writer.Write(code);
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
