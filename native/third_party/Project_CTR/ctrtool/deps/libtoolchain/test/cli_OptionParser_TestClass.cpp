#include <tc/Exception.h>
#include <fmt/core.h>

#include "cli_OptionParser_TestClass.h"

//---------------------------------------------------------

void cli_OptionParser_TestClass::runAllTests(void)
{
	fmt::print("[tc::cli::OptionParser] START\n");
	test_Constructor_DefaultConstructor();
	test_ProcessNoOptionsWithNoHandlers();
	test_ProcessOptionsWithNoHandlers();
	test_ProcessOptionsWithOnlyUnkHandler();
	test_ProcessOptionsWithLiteralHandlers();
	test_ProcessOptionsWithRegexHandlers();
	test_ProcessOptionsWithLiteralAndRegexHandlers();
	test_NullHandlerSupplied();
	test_RegularHandlerProvidesNoOptionLiteralOrRegex();
	test_ProcessMalformedOptions();
	fmt::print("[tc::cli::OptionParser] END\n");
}

//---------------------------------------------------------

void cli_OptionParser_TestClass::test_Constructor_DefaultConstructor()
{
	fmt::print("[tc::cli::OptionParser] test_Constructor_DefaultConstructor : ");
	try
	{
		try 
		{
			tc::cli::OptionParser opt;

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void cli_OptionParser_TestClass::test_ProcessNoOptionsWithNoHandlers()
{
	fmt::print("[tc::cli::OptionParser] test_ProcessNoOptionsWithNoHandlers : ");
	try
	{
		try 
		{
			tc::cli::OptionParser opt;

			std::vector<std::string> args;

			opt.processOptions(args);

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void cli_OptionParser_TestClass::test_ProcessOptionsWithNoHandlers()
{
	fmt::print("[tc::cli::OptionParser] test_ProcessOptionsWithNoHandlers : ");
	try
	{
		try 
		{
			tc::cli::OptionParser opt;

			std::vector<std::string> args = {"-someopt", "someparameter"};

			try 
			{
				opt.processOptions(args);
				throw tc::Exception("Did not throw an ArgumentException for unhandled option");
			}
			catch (const tc::ArgumentException&) { /* do nothing */ }
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void cli_OptionParser_TestClass::test_ProcessOptionsWithOnlyUnkHandler()
{
	fmt::print("[tc::cli::OptionParser] test_ProcessOptionsWithOnlyUnkHandler : ");
	try
	{
		enum class TestResult
		{
			Success,
			DidNotUseUnkOptionHandler,
			DidNotPassOptionsAndParameters
		};

		class UnkOptionHandler : public tc::cli::OptionParser::IOptionHandler
		{
		public:
			UnkOptionHandler(TestResult& external_result) :
				external_result(external_result)
			{}

			// this throws an exception as it should not be called
			const std::vector<std::string>& getOptionStrings() const
			{
				throw tc::Exception("getOptionStrings() not defined for UnkOptionHandler.");
			}

			// this throws an exception as it should not be called
			const std::vector<std::string>& getOptionRegexPatterns() const
			{
				throw tc::Exception("getOptionRegexPatterns() not defined for UnkOptionHandler.");
			}

			void processOption(const std::string& option, const std::vector<std::string>& params)
			{
				if (option == "-someopt" && params.size() == 1 && params[0] == "someparameter")
				{
					external_result = TestResult::Success;
				}
				else
				{
					external_result = TestResult::DidNotPassOptionsAndParameters;
				}
				
			}
		private:
			TestResult& external_result;
		};

		try 
		{
			tc::cli::OptionParser opt;

			TestResult result = TestResult::DidNotUseUnkOptionHandler;

			opt.registerUnrecognisedOptionHandler(std::make_shared<UnkOptionHandler>(UnkOptionHandler(result)));

			std::vector<std::string> args = {"-someopt", "someparameter"};

			opt.processOptions(args);

			if (result == TestResult::Success)
			{
				// all good
			}
			else if (result == TestResult::DidNotUseUnkOptionHandler)
			{
				throw tc::Exception("Unrecognised option handler was registered but not used.");
			}
			else if (result == TestResult::DidNotPassOptionsAndParameters)
			{
				throw tc::Exception("Unrecognised option handler was registered but the option & parameter were not passed to it correctly.");
			}
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void cli_OptionParser_TestClass::test_ProcessOptionsWithLiteralHandlers()
{
	fmt::print("[tc::cli::OptionParser] test_ProcessOptionsWithLiteralHandlers : ");
	try
	{
		enum class TestResult
		{
			Success,
			DidNotUseOptionHandler,
			DidNotPassOptionsAndParameters
		};

		class TestOptionHandler : public tc::cli::OptionParser::IOptionHandler
		{
		public:
			// The constructor is where you link the object to the state you want to modify in the call-back
			TestOptionHandler(TestResult& external_result, const std::string& opt_string, const std::vector<std::string>& expected_params) : 
				mExternalResult(external_result),
				mOptStrings({opt_string}),
				mOptRegex(),
				mExpectedParams(expected_params)
			{}
		
			const std::vector<std::string>& getOptionStrings() const
			{
				return mOptStrings;
			}
		
			const std::vector<std::string>& getOptionRegexPatterns() const
			{
				return mOptRegex;
			}
		
			void processOption(const std::string& option, const std::vector<std::string>& params)
			{
				if (option == mOptStrings[0] && params == mExpectedParams)
				{
					mExternalResult = TestResult::Success;
				}
				else
				{
					mExternalResult = TestResult::DidNotPassOptionsAndParameters;
				}
			}
		private:
			TestResult& mExternalResult;
			std::vector<std::string> mOptStrings;
			std::vector<std::string> mOptRegex;

			std::vector<std::string> mExpectedParams;
		};

		try 
		{
			tc::cli::OptionParser opt;

			TestResult flag_result = TestResult::DidNotUseOptionHandler;
			opt.registerOptionHandler(std::make_shared<TestOptionHandler>(TestOptionHandler(flag_result, "-flagoption", {})));

			TestResult single_param_result = TestResult::DidNotUseOptionHandler;
			opt.registerOptionHandler(std::make_shared<TestOptionHandler>(TestOptionHandler(single_param_result, "-singleparam", {"my_param"})));

			TestResult multi_param_result = TestResult::DidNotUseOptionHandler;
			opt.registerOptionHandler(std::make_shared<TestOptionHandler>(TestOptionHandler(multi_param_result, "-multiparam", {"my_param1", "my_param2"})));

			std::vector<std::string> args = {"-flagoption", "-singleparam", "my_param", "-multiparam", "my_param1", "my_param2"};

			opt.processOptions(args);

			if (flag_result == TestResult::Success)
			{
				// all good
			}
			else if (flag_result == TestResult::DidNotUseOptionHandler)
			{
				throw tc::Exception("Option handler (for flag option) was registered but not used.");
			}
			else if (flag_result == TestResult::DidNotPassOptionsAndParameters)
			{
				throw tc::Exception("Option handler (for flag option) was registered but the option & parameter were not passed to it correctly.");
			}

			if (single_param_result == TestResult::Success)
			{
				// all good
			}
			else if (single_param_result == TestResult::DidNotUseOptionHandler)
			{
				throw tc::Exception("Option handler (for single param option) was registered but not used.");
			}
			else if (single_param_result == TestResult::DidNotPassOptionsAndParameters)
			{
				throw tc::Exception("Option handler (for single param option) was registered but the option & parameter were not passed to it correctly.");
			}

			if (multi_param_result == TestResult::Success)
			{
				// all good
			}
			else if (multi_param_result == TestResult::DidNotUseOptionHandler)
			{
				throw tc::Exception("Option handler (for multi param option) was registered but not used.");
			}
			else if (multi_param_result == TestResult::DidNotPassOptionsAndParameters)
			{
				throw tc::Exception("Option handler (for multi param option) was registered but the option & parameter were not passed to it correctly.");
			}
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void cli_OptionParser_TestClass::test_ProcessOptionsWithRegexHandlers()
{
	fmt::print("[tc::cli::OptionParser] test_ProcessOptionsWithRegexHandlers : ");
	try
	{
		enum class TestResult
		{
			Success,
			DidNotUseOptionHandler,
			DidNotPassOptionsAndParameters
		};

		class TestOptionHandler : public tc::cli::OptionParser::IOptionHandler
		{
		public:
			// The constructor is where you link the object to the state you want to modify in the call-back
			TestOptionHandler(TestResult& external_result, const std::string& opt_regex, const std::string& expected_option, const std::vector<std::string>& expected_params) : 
				mExternalResult(external_result),
				mOptStrings(),
				mOptRegex({opt_regex}),
				mExpectedOption(expected_option),
				mExpectedParams(expected_params)
			{}
		
			const std::vector<std::string>& getOptionStrings() const
			{
				return mOptStrings;
			}
		
			const std::vector<std::string>& getOptionRegexPatterns() const
			{
				return mOptRegex;
			}
		
			void processOption(const std::string& option, const std::vector<std::string>& params)
			{
				if (option == mExpectedOption && params == mExpectedParams)
				{
					mExternalResult = TestResult::Success;
				}
				else
				{
					mExternalResult = TestResult::DidNotPassOptionsAndParameters;
				}
			}
		private:
			TestResult& mExternalResult;
			std::vector<std::string> mOptStrings;
			std::vector<std::string> mOptRegex;

			std::string mExpectedOption;
			std::vector<std::string> mExpectedParams;
		};

		try 
		{
			tc::cli::OptionParser opt;

			TestResult flag_result = TestResult::DidNotUseOptionHandler;
			opt.registerOptionHandler(std::make_shared<TestOptionHandler>(TestOptionHandler(flag_result, "(-F.a.+)", "-Flag", {})));

			TestResult single_param_result = TestResult::DidNotUseOptionHandler;
			opt.registerOptionHandler(std::make_shared<TestOptionHandler>(TestOptionHandler(single_param_result, "(-s.+)", "-singleparam", {"my_param"})));

			TestResult multi_param_result = TestResult::DidNotUseOptionHandler;
			opt.registerOptionHandler(std::make_shared<TestOptionHandler>(TestOptionHandler(multi_param_result, "(-multiparam)", "-multiparam", {"my_param1", "my_param2"})));

			std::vector<std::string> args = {"-Flag", "-singleparam", "my_param", "-multiparam", "my_param1", "my_param2"};

			opt.processOptions(args);

			if (flag_result == TestResult::Success)
			{
				// all good
			}
			else if (flag_result == TestResult::DidNotUseOptionHandler)
			{
				throw tc::Exception("Option handler (for flag option) was registered but not used.");
			}
			else if (flag_result == TestResult::DidNotPassOptionsAndParameters)
			{
				throw tc::Exception("Option handler (for flag option) was registered but the option & parameter were not passed to it correctly.");
			}

			if (single_param_result == TestResult::Success)
			{
				// all good
			}
			else if (single_param_result == TestResult::DidNotUseOptionHandler)
			{
				throw tc::Exception("Option handler (for single param option) was registered but not used.");
			}
			else if (single_param_result == TestResult::DidNotPassOptionsAndParameters)
			{
				throw tc::Exception("Option handler (for single param option) was registered but the option & parameter were not passed to it correctly.");
			}

			if (multi_param_result == TestResult::Success)
			{
				// all good
			}
			else if (multi_param_result == TestResult::DidNotUseOptionHandler)
			{
				throw tc::Exception("Option handler (for multi param option) was registered but not used.");
			}
			else if (multi_param_result == TestResult::DidNotPassOptionsAndParameters)
			{
				throw tc::Exception("Option handler (for multi param option) was registered but the option & parameter were not passed to it correctly.");
			}
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void cli_OptionParser_TestClass::test_ProcessOptionsWithLiteralAndRegexHandlers()
{
	fmt::print("[tc::cli::OptionParser] test_ProcessOptionsWithLiteralAndRegexHandlers : ");
	try
	{
		enum class TestResult
		{
			Success,
			DidNotUseOptionHandler,
			DidNotPassOptionsAndParameters
		};

		class TestOptionHandler : public tc::cli::OptionParser::IOptionHandler
		{
		public:
			// The constructor is where you link the object to the state you want to modify in the call-back
			TestOptionHandler(TestResult& external_result, const std::string& opt_literal, const std::string& opt_regex, const std::string& expected_option, const std::vector<std::string>& expected_params) : 
				mExternalResult(external_result),
				mOptStrings(opt_literal.empty() ? std::vector<std::string>({}) : std::vector<std::string>({opt_literal})),
				mOptRegex(opt_regex.empty() ? std::vector<std::string>({}) : std::vector<std::string>({opt_regex})),
				mExpectedOption(expected_option),
				mExpectedParams(expected_params)
			{}
		
			const std::vector<std::string>& getOptionStrings() const
			{
				return mOptStrings;
			}
		
			const std::vector<std::string>& getOptionRegexPatterns() const
			{
				return mOptRegex;
			}
		
			void processOption(const std::string& option, const std::vector<std::string>& params)
			{
				if (option == mExpectedOption && params == mExpectedParams)
				{
					mExternalResult = TestResult::Success;
				}
				else
				{
					mExternalResult = TestResult::DidNotPassOptionsAndParameters;
				}
			}
		private:
			TestResult& mExternalResult;
			std::vector<std::string> mOptStrings;
			std::vector<std::string> mOptRegex;

			std::string mExpectedOption;
			std::vector<std::string> mExpectedParams;
		};

		try 
		{
			tc::cli::OptionParser opt;

			TestResult regex_result = TestResult::DidNotUseOptionHandler;
			opt.registerOptionHandler(std::make_shared<TestOptionHandler>(TestOptionHandler(regex_result, "", "(-D.+)", "-DKEY", {"value"})));

			TestResult flag_result = TestResult::DidNotUseOptionHandler;
			opt.registerOptionHandler(std::make_shared<TestOptionHandler>(TestOptionHandler(flag_result, "-flag", "", "-flag", {})));

			TestResult single_param_result = TestResult::DidNotUseOptionHandler;
			opt.registerOptionHandler(std::make_shared<TestOptionHandler>(TestOptionHandler(single_param_result, "-singleparam", "", "-singleparam", {"my_param"})));

			TestResult multi_param_result = TestResult::DidNotUseOptionHandler;
			opt.registerOptionHandler(std::make_shared<TestOptionHandler>(TestOptionHandler(multi_param_result, "-multiparam", "", "-multiparam", {"my_param1", "my_param2"})));

			std::vector<std::string> args = {"-flag", "-DKEY=value", "-singleparam", "my_param", "-multiparam", "my_param1", "my_param2"};

			opt.processOptions(args);

			if (regex_result == TestResult::Success)
			{
				// all good
			}
			else if (regex_result == TestResult::DidNotUseOptionHandler)
			{
				throw tc::Exception("Option handler (for regex option) was registered but not used.");
			}
			else if (regex_result == TestResult::DidNotPassOptionsAndParameters)
			{
				throw tc::Exception("Option handler (for regex option) was registered but the option & parameter were not passed to it correctly.");
			}

			if (flag_result == TestResult::Success)
			{
				// all good
			}
			else if (flag_result == TestResult::DidNotUseOptionHandler)
			{
				throw tc::Exception("Option handler (for flag option) was registered but not used.");
			}
			else if (flag_result == TestResult::DidNotPassOptionsAndParameters)
			{
				throw tc::Exception("Option handler (for flag option) was registered but the option & parameter were not passed to it correctly.");
			}

			if (single_param_result == TestResult::Success)
			{
				// all good
			}
			else if (single_param_result == TestResult::DidNotUseOptionHandler)
			{
				throw tc::Exception("Option handler (for single param option) was registered but not used.");
			}
			else if (single_param_result == TestResult::DidNotPassOptionsAndParameters)
			{
				throw tc::Exception("Option handler (for single param option) was registered but the option & parameter were not passed to it correctly.");
			}

			if (multi_param_result == TestResult::Success)
			{
				// all good
			}
			else if (multi_param_result == TestResult::DidNotUseOptionHandler)
			{
				throw tc::Exception("Option handler (for multi param option) was registered but not used.");
			}
			else if (multi_param_result == TestResult::DidNotPassOptionsAndParameters)
			{
				throw tc::Exception("Option handler (for multi param option) was registered but the option & parameter were not passed to it correctly.");
			}
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void cli_OptionParser_TestClass::test_NullHandlerSupplied()
{
	fmt::print("[tc::cli::OptionParser] test_NullHandlerSupplied : ");
	try
	{
		try 
		{
			tc::cli::OptionParser opt;

			try 
			{
				opt.registerOptionHandler(std::shared_ptr<tc::cli::OptionParser::IOptionHandler>());
				throw tc::Exception(".registerOptionHandler() did not throw ArgumentNullException when passed a nullptr.");
			}
			catch (const tc::ArgumentNullException&)
			{
				// do nothing
			}

			try 
			{
				opt.registerUnrecognisedOptionHandler(std::shared_ptr<tc::cli::OptionParser::IOptionHandler>());
				throw tc::Exception(".registerUnrecognisedOptionHandler() did not throw ArgumentNullException when passed a nullptr.");
			}
			catch (const tc::ArgumentNullException&)
			{
				// do nothing
			}
			
			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void cli_OptionParser_TestClass::test_RegularHandlerProvidesNoOptionLiteralOrRegex()
{
	fmt::print("[tc::cli::OptionParser] test_RegularHandlerProvidesNoOptionLiteralOrRegex : ");
	try
	{
		enum class TestResult
		{
			Success,
			DidNotUseOptionHandler,
			DidNotPassOptionsAndParameters
		};

		class TestOptionHandler : public tc::cli::OptionParser::IOptionHandler
		{
		public:
			// The constructor is where you link the object to the state you want to modify in the call-back
			TestOptionHandler(TestResult& external_result, const std::string& opt_literal, const std::string& opt_regex, const std::string& expected_option, const std::vector<std::string>& expected_params) : 
				mExternalResult(external_result),
				mOptStrings(opt_literal.empty() ? std::vector<std::string>({}) : std::vector<std::string>({opt_literal})),
				mOptRegex(opt_regex.empty() ? std::vector<std::string>({}) : std::vector<std::string>({opt_regex})),
				mExpectedOption(expected_option),
				mExpectedParams(expected_params)
			{}
		
			const std::vector<std::string>& getOptionStrings() const
			{
				return mOptStrings;
			}
		
			const std::vector<std::string>& getOptionRegexPatterns() const
			{
				return mOptRegex;
			}
		
			void processOption(const std::string& option, const std::vector<std::string>& params)
			{
				if (option == mExpectedOption && params == mExpectedParams)
				{
					mExternalResult = TestResult::Success;
				}
				else
				{
					mExternalResult = TestResult::DidNotPassOptionsAndParameters;
				}
			}
		private:
			TestResult& mExternalResult;
			std::vector<std::string> mOptStrings;
			std::vector<std::string> mOptRegex;

			std::string mExpectedOption;
			std::vector<std::string> mExpectedParams;
		};

		try 
		{
			tc::cli::OptionParser opt;


			try
			{
				TestResult test_result = TestResult::DidNotUseOptionHandler;
				opt.registerOptionHandler(std::make_shared<TestOptionHandler>(TestOptionHandler(test_result, "", "", "-DKEY", {"value"})));
				throw tc::Exception(".registerOptionHandler() Did not throw tc::ArgumentOutOfRangeException when option handler had not option literals or option regex.");
			}
			catch (const tc::ArgumentOutOfRangeException&)
			{
				// do nothing
			}

			
			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}

void cli_OptionParser_TestClass::test_ProcessMalformedOptions()
{
	fmt::print("[tc::cli::OptionParser] test_ProcessMalformedOptions : ");
	try
	{
		class DummyOptionHandler : public tc::cli::OptionParser::IOptionHandler
		{
		public:
			// The constructor is where you link the object to the state you want to modify in the call-back
			DummyOptionHandler(const std::string& opt_literal, const std::string& opt_regex) : 
				mOptStrings(opt_literal.empty() ? std::vector<std::string>({}) : std::vector<std::string>({opt_literal})),
				mOptRegex(opt_regex.empty() ? std::vector<std::string>({}) : std::vector<std::string>({opt_regex}))
			{}
		
			const std::vector<std::string>& getOptionStrings() const
			{
				return mOptStrings;
			}
		
			const std::vector<std::string>& getOptionRegexPatterns() const
			{
				return mOptRegex;
			}
		
			void processOption(const std::string& option, const std::vector<std::string>& params)
			{
				// do nothing
			}
		private:
			std::vector<std::string> mOptStrings;
			std::vector<std::string> mOptRegex;
		};

		try 
		{
			try 
			{
				tc::cli::OptionParser opt;
				opt.processOptions({"dangling_parameter"});
				throw tc::Exception(".processOptions() did not throw exception for dangling parameter");
			}
			catch (const tc::ArgumentException&)
			{
				// do nothing
			}
			
			try 
			{
				tc::cli::OptionParser opt;

				opt.registerOptionHandler(std::make_shared<DummyOptionHandler>(DummyOptionHandler("-opt","")));
				opt.processOptions({"-opt=param", "dangling_parameter"});
				throw tc::Exception(".processOptions() did not throw exception for dangling parameter located after compound option");
			}
			catch (const tc::ArgumentException&)
			{
				// do nothing
			}

			fmt::print("PASS\n");
		}
		catch (const tc::Exception& e)
		{
			fmt::print("FAIL ({:s})\n", e.error());
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("UNHANDLED EXCEPTION ({:s})\n", e.what());
	}
}