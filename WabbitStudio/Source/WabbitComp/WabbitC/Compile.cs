using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Diagnostics;
using WabbitC.Model;

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
			tokenizer.Tokenize(fileContents);


			var preprocessorParser = new PreprocessorParser(tokenizer.Tokens);
			var preProcessorTokens = preprocessorParser.Parse();

            var tokenPass1 = new TokenPasses.ArrayDereference();
            var newTokens = tokenPass1.Run(preProcessorTokens);

            var tokenPass2 = new TokenPasses.Bracer();
            newTokens = tokenPass2.Run(newTokens);

            var tokens = newTokens.GetEnumerator();
            tokens.MoveNext();

            Module currentModule = Module.ParseModule(ref tokens);
            string code = currentModule.ToString();

            var writer = new StreamWriter("test_compiled.c");
            writer.Write(code);
            writer.Close();

/*			tokenizer = new Tokenizer();
			tokenizer.Tokenize("test + ( temp + 2*2)");
			var eval = new Expression(tokenizer.Tokens);
			var test = eval.Eval();
*/

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
