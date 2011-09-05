#pragma unmanaged

#pragma comment(lib, "spasm.lib")
#include "..\..\Source\SPASM\pass_one.h"
#include "..\..\Source\SPASM\pass_two.h"
#include "..\..\Source\SPASM\parser.h"
#include "..\..\Source\SPASM\spasm.h"
#include "..\..\Source\SPASM\errors.h"

#include <stdio.h>
#include <direct.h>
//#include <string>

#pragma managed

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace	Microsoft::VisualStudio::TestTools::UnitTesting;



using namespace std;

namespace SPASMTestsVS2008
{
	[TestClass]
	public ref class MismatchTests
	{
	public: 
		[TestInitialize]
		void Init()
		{
		}

		[TestCleanup]
		void Cleanup()
		{
		}

		// Run the file, return the error code
		int RunTest(const char *file_name)
		{
			ClearSPASMErrorSessions();
			char full_filename[256] = "..\\..\\..\\..\\..\\Tests\\SPASMTests\\";
			strcat_s(full_filename, file_name);

			OutputDebugString(TEXT("Start\n"));
			output_contents = (unsigned char *) malloc(output_buf_size);
			init_storage();
			curr_input_file = _strdup(full_filename);
			output_filename = "output.bin";
			mode = MODE_LIST;

			run_assembly();

			mode = 0;
			free_storage();
			free(output_contents);

			return (int) GetLastSPASMError();
		}

		[TestMethod]
		void Mismatch1()
		{
			RunTest("mismatch1.z80");
			Assert::AreEqual(SPASM_ERR_UNMATCHED_IF, (int) GetLastSPASMError(), "Wrong error code");
			Assert::AreEqual(1, GetLastSPASMErrorLine(), "Wrong error line");
		}
		[TestMethod]
		void Mismatch2()
		{
			RunTest("mismatch2.z80");
			Assert::AreEqual(SPASM_ERR_UNMATCHED_IF, (int) GetLastSPASMError(), "Wrong error code");
			Assert::AreEqual(5, GetLastSPASMErrorLine(), "Wrong error line");
		}
		[TestMethod]
		void Mismatch3()
		{
			Assert::AreEqual(SPASM_ERR_UNMATCHED_IF, RunTest("mismatch3.z80"));
		}
		[TestMethod]
		void Mismatch4()
		{
			Assert::AreEqual(SPASM_ERR_UNMATCHED_IF, RunTest("mismatch4.z80"));
		}
		[TestMethod]
		void Mismatch5()
		{
			Assert::AreEqual(SPASM_ERR_SUCCESS, RunTest("mismatch5.z80"));
		}
	};
}
