#pragma unmanaged

#pragma comment(lib, "spasm.lib")
#include "..\SPASM\pass_one.h"
#include "..\SPASM\pass_two.h"
#include "..\SPASM\parser.h"
#include "..\SPASM\spasm.h"
#include "..\SPASM\errors.h"
#include "..\SPASM\storage.h"

#include <stdio.h>

#pragma managed

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace	Microsoft::VisualStudio::TestTools::UnitTesting;

namespace SPASMTestsVS2008
{
	[TestClass]
	public ref class LabelTests
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
			output_contents = (unsigned char *) malloc(output_buf_size);
			init_storage();
			curr_input_file = strdup("..\\..\\..\\..\\..\\Tests\\SPASMTests\\Tests.asm");
			output_filename = "output.bin";
			mode = MODE_NORMAL;
			int nResult = run_assembly();
			Assert::AreEqual((int) EXIT_NORMAL, nResult, "Could not open test file");
		}

		[TestCleanup]
		void Cleanup()
		{
			free_storage();
			free(output_contents);
		}

		[TestMethod]
		void LocalLabel1()
		{
			char buffer[256] = "LocalLabel1()";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::IsFalse(error_occurred);

			bool success;
			int result;
			success = parse_num("-_", &result);
			Assert::IsTrue(success, "Failed to parse the local label");
			Assert::AreEqual(3, result);
		};

		[TestMethod]
		void LocalLabel2()
		{
			bool success;
			int result;

			pass_one = false;
			success = parse_num("_", &result);
			Assert::IsFalse(success, "Did not fail when it should have");
			Assert::AreEqual((DWORD) SPASM_ERR_LOCAL_LABEL_FORWARD_REF, GetLastSPASMError());
		};

		[TestMethod]
		void LocalLabel3()
		{
			char buffer[256] = "LocalLabel3()";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::IsFalse(error_occurred);

			bool success;
			int result;
			success = parse_num("-_+--_", &result);
			Assert::IsTrue(success, "Failed to parse the local label");
			Assert::AreEqual(8, result);
		};

		[TestMethod]
		void LocalLabel4()
		{
			out_ptr = output_contents;

			char buffer[256] = "LocalLabel4()";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::IsFalse(error_occurred);

			run_second_pass();

			Assert::AreEqual((unsigned char) 7, output_contents[2]);
		};

		
		[TestMethod]
		void Error104_org()
		{
			out_ptr = output_contents;

			char buffer[256] = ".org ";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::AreEqual((DWORD) SPASM_ERR_VALUE_EXPECTED, GetLastSPASMError());
		};

		[TestMethod]
		void Error107_org()
		{
			out_ptr = output_contents;

			char buffer[256] = ".org -1";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::AreEqual((DWORD) SPASM_ERR_INVALID_ADDRESS, GetLastSPASMError());
		};
	};
}
