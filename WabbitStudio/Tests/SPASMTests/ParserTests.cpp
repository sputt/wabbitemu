#pragma unmanaged

#pragma comment(lib, "spasm.lib")
#include <windows.h>

#include "..\..\Source\SPASM\pass_one.h"
#include "..\..\Source\SPASM\pass_two.h"
#include "..\..\Source\SPASM\parser.h"
#include "..\..\Source\SPASM\spasm.h"
#include "..\..\Source\SPASM\errors.h"

#pragma managed

#undef GetCurrentDirectory

using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace	Microsoft::VisualStudio::TestTools::UnitTesting;

namespace SPASMTestsVS2008
{
	[TestClass]
	public ref class ParserTests
	{
	private:
		TestContext^ testContextInstance;

	public: 
		/// <summary>
		///Gets or sets the test context which provides
		///information about and functionality for the current test run.
		///</summary>
		property Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ TestContext
		{
			Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ get()
			{
				return testContextInstance;
			}
			System::Void set(Microsoft::VisualStudio::TestTools::UnitTesting::TestContext^ value)
			{
				testContextInstance = value;
			}
		};

		[TestInitialize]
		void Init()
		{
			String ^cwd = Directory::GetCurrentDirectory();

			output_contents = (unsigned char *) malloc(OUTPUT_BUF_SIZE);
			init_storage();
			curr_input_file = "..\\..\\..\\..\\..\\Tests\\SPASMTests\\Tests.asm";
			output_filename = "output.bin";
			mode = MODE_NORMAL;
			int nResult = run_assembly();
			DWORD dwLastError = GetLastSPASMError();
			System::Diagnostics::Debug::WriteLine(cwd);
			Assert::AreEqual((int) EXIT_NORMAL, nResult, "Could not open test file");
		}

		[TestCleanup]
		void Cleanup()
		{
			free_storage();
			free(output_contents);
		}

		[TestMethod]
		void Math1()
		{
			
			bool success;
			int result;
			success = parse_num("1+1", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(2, result);
		};
		[TestMethod]
		void Math2()
		{
			
			bool success;
			int result;
			success = parse_num("((1+1)*2+3)/2)*4+3", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(15, result);
		};
		[TestMethod]
		void MathOoO()
		{
			bool success;
			int result;
			success = parse_num("1+2/2", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(1, result);
		};
		[TestMethod]
		void Math3()
		{
			bool success;
			int result;
			success = parse_num("(1+1)+(5%2)+(4/4)+(2^1)", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(7, result);
		};
		[TestMethod]
		void BadMath1()
		{
			bool success;
			int result;
			success = parse_num("1+1(", &result);
			Assert::IsFalse(success);
		};
		[TestMethod]
		void BadMath2()
		{
			bool success;
			int result;
			success = parse_num("1+made_up_label", &result);
			Assert::IsFalse(success);
		};
		[TestMethod]
		void Whitespace()
		{
			bool success;
			int result;
			success = parse_num("1+1     +1\t+1  +1+ 1\t\t+1", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(7, result);
		};
		[TestMethod]
		void Ascii()
		{
			bool success;
			int result;
			success = parse_num("'a'+'b'", &result);
			Assert::IsTrue(success);
			Assert::AreEqual('a'+'b', result);
		};

		[TestMethod]
		void ParserError103()
		{
			bool success;
			int result;
			success = parse_num("1 1", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD) SPASM_ERR_OPERATOR_EXPECTED, GetLastSPASMError());
		};

		[TestMethod]
		void ParserError105()
		{
			bool success;
			int result;
			success = parse_num("1 + ^1", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD) SPASM_ERR_BAD_VALUE_PREFIX, GetLastSPASMError());
		};

		[TestMethod]
		void ParserError102()
		{
			bool success;
			int result;
			pass_one = true;
			success = parse_num("ParserError102()", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD) SPASM_ERR_ARG_USED_WITHOUT_VALUE, GetLastSPASMError());
		};

		[TestMethod]
		void ParserError200()
		{
			bool success;
			int result;
			pass_one = true;
			success = parse_num("20a", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD) SPASM_ERR_INVALID_DECIMAL_DIGIT, GetLastSPASMError());
		};

		[TestMethod]
		void ParserError201()
		{
			bool success;
			int result;
			pass_one = true;
			success = parse_num("$20ag", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD) SPASM_ERR_INVALID_HEX_DIGIT, GetLastSPASMError());
		};

		[TestMethod]
		void ParserError202()
		{
			bool success;
			int result;
			pass_one = true;
			success = parse_num("%10120", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD) SPASM_ERR_INVALID_BINARY_DIGIT, GetLastSPASMError());
		};

		[TestMethod]
		void ParserError115()
		{
			bool success;
			int result;
			pass_one = true;
			success = parse_num("InfiniteRecursiveDefine", &result);
			Assert::IsFalse(success);
			Assert::AreEqual((DWORD) SPASM_ERR_EXCEEDED_RECURSION_LIMIT, GetLastSPASMError());
		};
	};
}
