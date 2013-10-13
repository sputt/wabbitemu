#pragma unmanaged

#pragma comment(lib, "spasm.lib")
#include "..\SPASM\pass_one.h"
#include "..\SPASM\pass_two.h"
#include "..\SPASM\parser.h"
#include "..\SPASM\spasm.h"
#include "..\SPASM\errors.h"
#include "..\SPASM\storage.h"

#include <stdio.h>
#include <direct.h>

#pragma managed

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace	Microsoft::VisualStudio::TestTools::UnitTesting;

namespace SPASMTestsVS2008
{
	[TestClass]
	public ref class MacroTests
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
			ClearSPASMErrorSessions();

			output_contents = (unsigned char *) malloc(output_buf_size);
			init_storage();
			curr_input_file = strdup("..\\..\\..\\..\\..\\Tests\\SPASMTests\\Tests.asm");
			output_filename = "output.bin";
			mode = MODE_NORMAL;

			char buffer[256];
			_getcwd(buffer, sizeof(buffer));

			int nResult = run_assembly();
			Assert::AreEqual((int) EXIT_NORMAL, nResult, "Could not open test file");

			ClearSPASMErrorSessions();
		}

		[TestCleanup]
		void Cleanup()
		{
			free_storage();
			free(output_contents);
		}

		[TestMethod]
		void Macro1()
		{
			bool success;
			int result;
			success = parse_num("Macro1()+1", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(2, result);
		};

		[TestMethod]
		void Macro2()
		{
			bool success;
			int result;
			success = parse_num("Macro1+1", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(2, result);
		};

		[TestMethod]
		void Macro3()
		{
			bool success;
			int result;
			success = parse_num("Macro1()+Macro1+Macro1( )+Macro1()", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(4, result);
		};

		[TestMethod]
		void RecursiveMacro1()
		{
			bool success;
			int result;
			success = parse_num("RecursiveMacro1(6)", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(6, result);
		};

		[TestMethod]
		void FibonacciMacro0()
		{
			bool success;
			int result;
			success = parse_num("FibonacciMacro(0)", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(0, result);
		};

		[TestMethod]
		void FibonacciMacro1()
		{
			bool success;
			int result;
			success = parse_num("FibonacciMacro(1)", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(1, result);
		};

		[TestMethod]
		void FibonacciMacro12()
		{
			bool success;
			int result;
			success = parse_num("FibonacciMacro(12)", &result);
			Assert::IsTrue(success);
			Assert::AreEqual(144, result);
		};

		[TestMethod]
		void EchoMacro1()
		{
			char buffer[256] = " EchoMacro1()";
			run_first_pass(buffer);

			System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("EchoMacro1.txt");
			System::String ^str = sr->ReadLine();

			Assert::AreEqual(gcnew String("Hello World"), str);
		};

		[TestMethod]
		void EchoMacro2()
		{
			char buffer[256] = " EchoMacro2(\"Test string 2\")";
			run_first_pass(buffer);

			System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("EchoMacro2.txt");
			System::String ^str = sr->ReadLine();

			Assert::AreEqual(gcnew String("Test string 2"), str);
		};

		[TestMethod]
		void EchoMacro3()
		{
			char buffer[256] = " EchoMacro3(11, \"Test string 3\")";
			run_first_pass(buffer);

			System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("EchoMacro3.txt");
			System::String ^str = sr->ReadLine();

			Assert::AreEqual(gcnew String("My prefix (89): Test string 3"), str);
		};

		[TestMethod]
		void EchoMacro4()
		{
			char buffer[256] = " .echomacro4 11, \"Test string 4\"";
			error_occurred = false;
			run_first_pass(buffer);

			Assert::IsFalse(error_occurred);

			System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("EchoMacro4.txt");
			System::String ^str = sr->ReadLine();

			Assert::AreEqual(gcnew String("My prefix (11): Test string 4"), str);
		};

		[TestMethod]
		void EmitDefine1()
		{
			char buffer[256] = " .db EmitDefine1";
			int session = StartSPASMErrorSession();
			run_first_pass(buffer);
			Assert::AreEqual(false, IsSPASMErrorSessionFatal(session), "Errors occurred");
			ReplaySPASMErrorSession(session);
			EndSPASMErrorSession(session);

			Assert::AreEqual((BYTE) 10, output_contents[0], "Output value was wrong");
		};

		[TestMethod]
		void EmitDefine2()
		{
			char buffer[256] = " .db EmitDefine2";
			int session = StartSPASMErrorSession();
			run_first_pass(buffer);
			Assert::AreEqual(false, IsSPASMErrorSessionFatal(session), "Errors occurred");
			ReplaySPASMErrorSession(session);
			EndSPASMErrorSession(session);

			Assert::AreEqual((BYTE) 33, (BYTE) (output_contents[0]+output_contents[1]+output_contents[2]), "Output value was wrong");
		};

		[TestMethod]
		void EmitDefine3()
		{
			char buffer[256] = " .db EmitDefine3";
			int session = StartSPASMErrorSession();
			run_first_pass(buffer);
			Assert::AreEqual(false, IsSPASMErrorSessionFatal(session), "Errors occurred");
			ReplaySPASMErrorSession(session);
			EndSPASMErrorSession(session);

			Assert::AreEqual((BYTE) 33, (BYTE) (output_contents[0]+output_contents[1]+output_contents[2]), "Output value was wrong");
		};

		// This test verifies that errors that occur parsing element get passed up
		[TestMethod]
		void EmitDefine4()
		{
			char buffer[256] = " .db EmitDefine4";
			int session = StartSPASMErrorSession();
			run_first_pass(buffer);
			Assert::AreEqual((DWORD) SPASM_ERR_VALUE_EXPECTED, GetLastSPASMError(), "Error did not occur when it should have");
			ReplaySPASMErrorSession(session);
			EndSPASMErrorSession(session);
		};

		// This test verifies that string defines are correctly parsed
		[TestMethod]
		void EmitDefine5()
		{
			char buffer[256] = " .db EmitDefine5";
			int session = StartSPASMErrorSession();
			run_first_pass(buffer);
			ReplaySPASMErrorSession(session);
			Assert::AreEqual(false, IsSPASMErrorSessionFatal(session), "Errors occurred");
			EndSPASMErrorSession(session);

			Assert::AreEqual((BYTE) 'A', (BYTE) output_contents[0], "Output value was wrong");
		};

		[TestMethod]
		void ConcatMacro1()
		{
			char buffer[256] = " ConcatMacro1(12)";
			error_occurred = false;
			run_first_pass(buffer);

			Assert::IsFalse(error_occurred);

			System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("ConcatMacro1.txt");
			System::String ^str = sr->ReadLine();

			Assert::AreEqual(gcnew String("$C"), str);
		};

		[TestMethod]
		void ConcatMacro2()
		{
			char buffer[256] = " ConcatMacro2(test)";
			error_occurred = false;
			run_first_pass(buffer);

			Assert::IsFalse(error_occurred);

			System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("ConcatMacro2.txt");
			System::String ^str = sr->ReadLine();

			Assert::AreEqual(gcnew String("test"), str);
		};

		[TestMethod]
		void ConcatMacro3()
		{
			char buffer[256] = " ConcatMacro3(6)";
			error_occurred = false;
			run_first_pass(buffer);

			Assert::IsFalse(error_occurred);

			System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("ConcatMacro3.txt");
			System::String ^str = sr->ReadLine();

			Assert::AreEqual(gcnew String("XXXXXX"), str);
		};

		[TestMethod]
		void ConcatMacro4()
		{
			char buffer[256] = " ConcatMacro4(8)";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::IsFalse(error_occurred);

			System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("ConcatMacro4.txt");
			System::String ^str = sr->ReadLine();

			Assert::AreEqual(gcnew String("XXXXXXXX"), str);
		};

		[TestMethod]
		void ConcatMacro5()
		{
			char buffer[256] = " ConcatMacro5()";
			error_occurred = false;
			run_first_pass(buffer);
			Assert::IsFalse(error_occurred);

			System::IO::StreamReader ^sr = gcnew System::IO::StreamReader("ConcatMacro5.txt");
			System::String ^str = sr->ReadLine();

			Assert::AreEqual(gcnew String("XXXXXXX"), str);
		};

		void IsWithinRange(int v1, int v2)
		{
			int diff = abs(v1 - v2);
			char str[256];
			sprintf_s(str, "Value of off by %d", v2 - v1);
			Assert::IsTrue(diff < 15, gcnew String(str));
		}

		[TestMethod]
		void Fixed14Power2()
		{
			bool success;
			int result;
			success = parse_num("Fixed14Power(25735, 2)", &result);
			Assert::IsTrue(success);
			IsWithinRange(40425, result);
		};

		[TestMethod]
		void Fixed14Power3()
		{
			bool success;
			int result;
			success = parse_num("Fixed14Power(25735, 3)", &result);
			Assert::IsTrue(success);
			IsWithinRange(63500, result);
		};

		[TestMethod]
		void Fixed14Power4()
		{
			bool success;
			int result;
			success = parse_num("Fixed14Power(25735, 4)", &result);
			Assert::IsTrue(success);
			IsWithinRange(99732, result);
		};

		[TestMethod]
		void Fixed14Cos15()
		{
			bool success;
			int result;
			success = parse_num("Fixed14Cos(15)", &result);
			Assert::IsTrue(success);
			IsWithinRange(System::Math::Cos(15.0 * System::Math::PI / 180.0) * 16384.0, result);
		};

		[TestMethod]
		void Fixed14Cos30()
		{
			bool success;
			int result;
			success = parse_num("Fixed14Cos(30)", &result);
			Assert::IsTrue(success);
			IsWithinRange(System::Math::Cos(30.0 * System::Math::PI / 180.0) * 16384.0, result);
		};

		[TestMethod]
		void Fixed14Cos45()
		{
			bool success;
			int result;
			success = parse_num("Fixed14Cos(45)", &result);
			Assert::IsTrue(success);
			IsWithinRange(System::Math::Cos(45.0 * System::Math::PI / 180.0) * 16384.0, result);
		};

		[TestMethod]
		void Fixed14Cos60()
		{
			bool success;
			int result;
			success = parse_num("Fixed14Cos(60)", &result);
			Assert::IsTrue(success);
			IsWithinRange(System::Math::Cos(60.0 * System::Math::PI / 180.0) * 16384.0, result);
		};

		[TestMethod]
		void Fixed14Cos75()
		{
			bool success;
			int result;
			success = parse_num("Fixed14Cos(75)", &result);
			Assert::IsTrue(success);
			IsWithinRange(System::Math::Cos(75.0 * System::Math::PI / 180.0) * 16384.0, result);
		};

		[TestMethod]
		void Fixed14Cos90()
		{
			bool success;
			int result;
			success = parse_num("Fixed14Cos(90)", &result);
			Assert::IsTrue(success);
			IsWithinRange(System::Math::Cos(90.0 * System::Math::PI / 180.0) * 16384.0, result);
		};
	};
}
