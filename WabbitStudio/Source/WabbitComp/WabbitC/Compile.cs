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

		private static void WriteModule(Module module, int pass)
		{
			string code = module.ToString();

			var writer = new StreamWriter("intermediate." + pass + ".c");
			writer.Write(code.Replace("\r\n", "\n").Replace("\n", "\r\n"));
			writer.Close();
		}

		/// <summary>
		/// Parses a file and outputs a .asm file
		/// </summary>
		/// <returns></returns>
		public static bool DoCompileFile(string inputFile, OptimizeLevel opLevel = OptimizeLevel.OptimizeNone, PassCount passCount = PassCount.PassAlls)
		{
			optimizeLevel = opLevel;
			string fileContents = TryOpenFile(inputFile);
			if (string.IsNullOrEmpty(fileContents))
				return false;
			var module = DoCompile(fileContents);
			
			var asmWriter = new StreamWriter(Path.GetFileNameWithoutExtension(inputFile) + "_compiled.z80");
			asmWriter.Write(AssemblyGenerator.GenerateCode(ref module));
			asmWriter.Close();

			string outputFile = inputFile.Replace("expected", "actual");
			if (outputFile.Equals(inputFile))
			{
				outputFile = Path.GetFileNameWithoutExtension(inputFile) + "_compiled.c";
			}

			var writer = new StreamWriter(outputFile);
			writer.Write(module.ToString().Replace("\r\n", "\n").Replace("\n", "\r\n"));
			writer.Close();

			return true;
		}

		static Module DoCompile(string fileContents, OptimizeLevel opLevel = OptimizeLevel.OptimizeNone, PassCount passCount = PassCount.PassAlls)
		{
			var tokenizerTokens = Tokenizer.Tokenize(fileContents);

			var preprocessorParser = new PreprocessorParser(tokenizerTokens);
			var preProcessorTokens = preprocessorParser.Parse();

			var tokenPassTokens = RunTokenPasses(preProcessorTokens);

			var tokens = tokenPassTokens.GetEnumerator();
			tokens.MoveNext();

			var currentModule = Module.ParseModule(ref tokens);

            if (optimizeLevel != OptimizeLevel.OptimizeNone)
            {
                Optimizer.Loop.Optimizer.RunOptimizer(ref currentModule, optimizeLevel);
            }

			WriteModule(currentModule, 1);

			// Statement passes
			if (passCount >= PassCount.Pass2)
			{
				StatementPasses.BlockCollapse.Run(currentModule);
				StatementPasses.IfGotoConversion.Run(currentModule);
				StatementPasses.LoopGotoConversion.Run(currentModule);
				StatementPasses.ReorderDeclarations.Run(currentModule);
				StatementPasses.MarkRecursiveFunctions.Run(currentModule);
			}
			WriteModule(currentModule, 2);

			//if (optimizeLevel != OptimizeLevel.OptimizeNone)
			//	Optimizer.Optimizer.RunOptimizer(ref currentModule, optimizeLevel);

			WriteModule(currentModule, 3);

			if (passCount >= PassCount.Pass3)
			{
				//StatementPasses.ReplaceLocalsWithGlobals.Run(currentModule);
				StatementPasses.ApplyCallingConvention.Run(currentModule);
				StatementPasses.LabelMerger.Run(currentModule);
				StatementPasses.RemovePointlessGotos.Run(currentModule);

				// Adjust the arithmetic
				StatementPasses.ConvertAddSubToIncDec.Run(currentModule);
				StatementPasses.RemoveMathImmediates.Run(currentModule);

				StatementPasses.RegisterAllocator.DumbRegisterAllocator.Run(currentModule);
			}

			currentModule.IntermediateStrings.Add("#include <string.h>");
			string code = currentModule.ToString();
			var asmCode = AssemblyGenerator.GenerateCode(ref currentModule);

			return currentModule;
		}

		//TODO: make more descriptive
		public enum PassCount
		{
			Pass1 = 1,
			Pass2 = 2,
			Pass3 = 3,
			PassAlls = Int32.MaxValue
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
