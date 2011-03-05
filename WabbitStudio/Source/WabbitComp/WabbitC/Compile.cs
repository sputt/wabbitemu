using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Diagnostics;

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

            Tokenizer tokenizer = new Tokenizer();
			tokenizer.Tokenize("temp + 3*4");

			Expression exp = new Expression(tokenizer.Tokens);
			exp.Eval();
			PreprocessorParser preprocessorParser = new PreprocessorParser(tokenizer.Tokens);
			preprocessorParser.Parse();
            /*Parser parser = new Parser();
            parser.ParseTokens(tokenizer.Tokens);*/

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
