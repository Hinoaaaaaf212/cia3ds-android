#include <tc/Exception.h>
#include <tc/io/PathUtil.h>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include "io_Path_TestClass.h"

//---------------------------------------------------------

void io_Path_TestClass::runAllTests(void)
{
	fmt::print("[tc::io::Path] START\n");
	test_Constructor_Default();
	test_Constructor_InitializerList();
	test_Constructor_u8string();
	test_Constructor_u16string();
	test_Constructor_u32string();
	test_Method_pop_front();
	test_Method_pop_back();
	test_Method_push_front();
	test_Method_push_back();
	test_Method_clear();
	test_Method_substr_withPosLen();
	test_Method_substr_withIterator();
	test_Method_to_string();
	test_Method_to_u16string();
	test_Method_to_u32string();
	test_Operator_string();
	test_Operator_u16string();
	test_Operator_u32string();
	test_Operator_Addition();
	test_Operator_Append();
	test_Scenario_AppendStressTest();
	test_Operator_EqualityTest();
	test_Operator_InequalityTest();
	test_Operator_LessThanTest();
	fmt::print("[tc::io::Path] END\n");
}

//---------------------------------------------------------

void io_Path_TestClass::test_Constructor_Default()
{
	fmt::print("[tc::io::Path] test_Constructor_Default : ");
	try
	{
		tc::io::Path path_empty;
		
		if (path_empty.size() != 0)
		{
			throw tc::Exception("Default constructor did not create a path with 0 elements (.size() != 0)");
		}

		if (path_empty.empty() != true)
		{
			throw tc::Exception("Default constructor did not create an empty path (.empty() != true)");
		}

		if (path_empty.begin() != path_empty.end())
		{
			throw tc::Exception("Default constructor did not create an empty path (.begin() == .end())");
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Constructor_InitializerList()
{
	fmt::print("[tc::io::Path] test_Constructor_InitializerList : ");
	try
	{
		tc::io::Path path = {"a", "path", "from", "an", "initializer", "list"};
		std::list<std::string> expected_elements = {"a", "path", "from", "an", "initializer", "list"};
		
		if (path.size() != expected_elements.size())
		{
			throw tc::Exception(fmt::format("InitializerList constructor did not create a path with {} elements (.size() == {})", expected_elements.size(), path.size()));
		}

		if (path.empty() != false)
		{
			throw tc::Exception("InitializerList constructor created an empty path (.empty() != false)");
		}

		if (path.begin() == path.end())
		{
			throw tc::Exception("InitializerList constructor created an empty path (.begin() == .end())");
		}

		for (auto pathItr = path.begin(), expItr = expected_elements.begin(); pathItr != path.end(); pathItr++, expItr++)
		{
			if (*pathItr != *expItr)
			{
				throw tc::Exception(fmt::format("InitializerList constructer created a path with unexpected elements (Path contained {}, expected {})", std::list<std::string>(path.begin(), path.end()), expected_elements) );
			}
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Constructor_u8string()
{
	fmt::print("[tc::io::Path] test_Constructor_u8string : ");
	try
	{
		struct Test
		{
			std::string path_string;
			tc::io::Path expected_path;
		};
		std::vector<Test> tests {
			{"", tc::io::Path({})},
			{"/", tc::io::Path({""})},
			{"C:\\", tc::io::Path({"C:"})},
			{"~/.ssh/id_rsa", tc::io::Path({"~", ".ssh", "id_rsa"})},
			{"C:\\Users\\Administrator\\Desktop", tc::io::Path({"C:", "Users", "Administrator", "Desktop"})},
		};

		for (auto itr = tests.begin(); itr != tests.end(); itr++)
		{
			tc::io::Path path = itr->path_string;
			if (path != itr->expected_path)
			{
				throw tc::Exception(fmt::format("Failed to convert \"{}\" to path. (Path contained {}, expected {})", itr->path_string, std::list<std::string>(path.begin(), path.end()), std::list<std::string>(itr->expected_path.begin(), itr->expected_path.end())));
			}
		}

		try
		{
			tc::io::Path("A/Mix\\Of/Path\\Separators");
			throw tc::Exception("Path(const std::string&) constructor did not throw tc::ArgumentException where there was a mix of path separators.");
		}
		catch(const tc::ArgumentException&)
		{
			// do nothing
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Constructor_u16string()
{
	fmt::print("[tc::io::Path] test_Constructor_u16string : ");
	try
	{
		struct Test
		{
			std::u16string path_string;
			tc::io::Path expected_path;
		};
		std::vector<Test> tests {
			{u"", tc::io::Path({})},
			{u"/", tc::io::Path({""})},
			{u"C:\\", tc::io::Path({"C:"})},
			{u"~/.ssh/id_rsa", tc::io::Path({"~", ".ssh", "id_rsa"})},
			{u"C:\\Users\\Administrator\\Desktop", tc::io::Path({"C:", "Users", "Administrator", "Desktop"})},
		};

		for (auto itr = tests.begin(); itr != tests.end(); itr++)
		{
			tc::io::Path path = itr->path_string;
			if (path != itr->expected_path)
			{
				std::string u8_string;
				tc::string::TranscodeUtil::UTF16ToUTF8(itr->path_string, u8_string);
				throw tc::Exception(fmt::format("Failed to convert u\"{}\" to path. (Path contained {}, expected {})", u8_string, std::list<std::string>(path.begin(), path.end()), std::list<std::string>(itr->expected_path.begin(), itr->expected_path.end())));
			}
		}

		try
		{
			tc::io::Path(u"A/Mix\\Of/Path\\Separators");
			throw tc::Exception("Path(const std::u16string&) constructor did not throw tc::ArgumentException where there was a mix of path separators.");
		}
		catch(const tc::ArgumentException&)
		{
			// do nothing
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Constructor_u32string()
{
	fmt::print("[tc::io::Path] test_Constructor_u32string : ");
	try
	{
		struct Test
		{
			std::u32string path_string;
			tc::io::Path expected_path;
		};
		std::vector<Test> tests {
			{U"", tc::io::Path({})},
			{U"/", tc::io::Path({""})},
			{U"C:\\", tc::io::Path({"C:"})},
			{U"~/.ssh/id_rsa", tc::io::Path({"~", ".ssh", "id_rsa"})},
			{U"C:\\Users\\Administrator\\Desktop", tc::io::Path({"C:", "Users", "Administrator", "Desktop"})},
		};

		for (auto itr = tests.begin(); itr != tests.end(); itr++)
		{
			tc::io::Path path = itr->path_string;
			if (path != itr->expected_path)
			{
				std::string u8_string;
				tc::string::TranscodeUtil::UTF32ToUTF8(itr->path_string, u8_string);
				throw tc::Exception(fmt::format("Failed to convert U\"{}\" to path. (Path contained {}, expected {})", u8_string, std::list<std::string>(path.begin(), path.end()), std::list<std::string>(itr->expected_path.begin(), itr->expected_path.end())));
			}
		}

		try
		{
			tc::io::Path(U"A/Mix\\Of/Path\\Separators");
			throw tc::Exception("Path(const std::u32string&) constructor did not throw tc::ArgumentException where there was a mix of path separators.");
		}
		catch(const tc::ArgumentException&)
		{
			// do nothing
		}
		

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Method_pop_front()
{
	fmt::print("[tc::io::Path] test_Method_pop_front : ");
	try
	{
		tc::io::Path path("a/b/c/d/e/f/g/h/i/j");
		std::list<std::string> expectedElements = {"a","b","c","d","e","f","g","h","i","j"};

		for (size_t i = 0; i < 10; i++, path.pop_front(), expectedElements.pop_front())
		{
			if (*path.begin() != *expectedElements.begin())
			{
				throw tc::Exception("pop_front() did not place expected element at begin()");
			}
			if (path.front() != expectedElements.front())
			{
				throw tc::Exception("pop_front() did not place expected element at front()");
			}
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Method_pop_back()
{
	fmt::print("[tc::io::Path] test_Method_pop_back : ");
	try
	{
		tc::io::Path path("a/b/c/d/e/f/g/h/i/j");
		std::list<std::string> expectedElements = {"a","b","c","d","e","f","g","h","i","j"};

		for (size_t i = 0; i < 10; i++, path.pop_back(), expectedElements.pop_back())
		{
			if (*(--path.end()) != *(--expectedElements.end()))
			{
				throw tc::Exception("pop_back() did not place expected element at (--end())");
			}
			if (path.back() != expectedElements.back())
			{
				throw tc::Exception("pop_back() did not place expected element at back())");
			}
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Method_push_front()
{
	fmt::print("[tc::io::Path] test_Method_push_front : ");
	try
	{
		tc::io::Path path("");
		std::list<std::string> expectedElements = {};

		std::list<std::string> pushCue = {"a","b","c","d","e","f","g","h","i","j"};

		for (size_t i = 0; i < 10; i++, pushCue.pop_front())
		{
			path.push_front(*(pushCue.begin()));
			expectedElements.push_front(*(pushCue.begin()));
			if (*(path.begin()) != *(expectedElements.begin()))
			{
				throw tc::Exception("push_front() did not place expected element at *(begin())");
			}
			if (path.front() != expectedElements.front())
			{
				throw tc::Exception("push_front() did not place expected element at front()");
			}
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Method_push_back()
{
	fmt::print("[tc::io::Path] test_Method_push_back : ");
	try
	{
		tc::io::Path path("");
		std::list<std::string> expectedElements = {};

		std::list<std::string> pushCue = {"a","b","c","d","e","f","g","h","i","j"};

		for (size_t i = 0; i < 10; i++, pushCue.pop_front())
		{
			path.push_back(*(pushCue.begin()));
			expectedElements.push_back(*(pushCue.begin()));
			if (*(--path.end()) != *(--expectedElements.end()))
			{
				throw tc::Exception("push_back() did not place expected element at *(--end())");
			}
			if (path.back() != expectedElements.back())
			{
				throw tc::Exception("push_back() did not place expected element at back())");
			}
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Method_clear()
{
	fmt::print("[tc::io::Path] test_Method_clear : ");
	try
	{
		tc::io::Path path("a/b/c/d/e/f/g/h/i/j");
		
		path.clear();

		if (path.size() != 0)
		{
			throw tc::Exception(".size() did not have expected value of 0 after .clear()");
		}

		if (path.empty() != true)
		{
			throw tc::Exception(".empty() did not have expected value of true after .clear()");
		}

		if (path.begin() != path.end())
		{
			throw tc::Exception(".begin() != .end() after .clear()");
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Method_substr_withPosLen()
{
	fmt::print("[tc::io::Path] test_Method_substr_withPosLen : ");
	try
	{
		struct Test
		{
			tc::io::Path path;
			size_t pos;
			size_t len;
			tc::io::Path exp_subpath;
		};
		std::vector<Test> tests {
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, tc::io::Path::npos, {"a","b","c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 1, tc::io::Path::npos, {"b","c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 2, tc::io::Path::npos, {"c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 3, tc::io::Path::npos, {"d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 4, tc::io::Path::npos, {"e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 5, tc::io::Path::npos, {"f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 6, tc::io::Path::npos, {"g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 7, tc::io::Path::npos, {"h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 8, tc::io::Path::npos, {"i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 9, tc::io::Path::npos, {"j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 10, tc::io::Path::npos, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 11, tc::io::Path::npos, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 12, tc::io::Path::npos, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 13, tc::io::Path::npos, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 12, {"a","b","c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 11, {"a","b","c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 10, {"a","b","c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 9, {"a","b","c","d","e","f","g","h","i"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 8, {"a","b","c","d","e","f","g","h"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 7, {"a","b","c","d","e","f","g"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 6, {"a","b","c","d","e","f"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 5, {"a","b","c","d","e"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 4, {"a","b","c","d"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 3, {"a","b","c"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 2, {"a","b"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 1, {"a"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 0, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 1, 4, {"b","c","d","e"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 2, 4, {"c","d","e","f"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 3, 4, {"d","e","f","g"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 4, 4, {"e","f","g","h"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 5, 4, {"f","g","h","i"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 6, 4, {"g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 7, 4, {"h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 8, 4, {"i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 9, 4, {"j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 10, 4, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, tc::io::Path::npos, tc::io::Path::npos, {}},
		};

		for (auto itr = tests.begin(); itr != tests.end(); itr++)
		{
			tc::io::Path subpath = itr->path.subpath(itr->pos, itr->len);

			if (subpath != itr->exp_subpath)
			{
				throw tc::Exception(fmt::format(".subpath(pos={},len={}) returned incorrect sub path (returned {}, expected {})", itr->pos, itr->len, std::list<std::string>(subpath.begin(), subpath.end()), std::list<std::string>(itr->exp_subpath.begin(), itr->exp_subpath.end())));
			}
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Method_substr_withIterator()
{
	fmt::print("[tc::io::Path] test_Method_substr_withIterator : ");
	try
	{
		struct Test
		{
			tc::io::Path path;
			size_t pos;
			size_t len;
			tc::io::Path exp_subpath;
		};
		std::vector<Test> tests {
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, tc::io::Path::npos, {"a","b","c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 1, tc::io::Path::npos, {"b","c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 2, tc::io::Path::npos, {"c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 3, tc::io::Path::npos, {"d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 4, tc::io::Path::npos, {"e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 5, tc::io::Path::npos, {"f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 6, tc::io::Path::npos, {"g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 7, tc::io::Path::npos, {"h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 8, tc::io::Path::npos, {"i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 9, tc::io::Path::npos, {"j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 10, tc::io::Path::npos, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 11, tc::io::Path::npos, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 12, tc::io::Path::npos, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 13, tc::io::Path::npos, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 12, {"a","b","c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 11, {"a","b","c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 10, {"a","b","c","d","e","f","g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 9, {"a","b","c","d","e","f","g","h","i"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 8, {"a","b","c","d","e","f","g","h"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 7, {"a","b","c","d","e","f","g"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 6, {"a","b","c","d","e","f"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 5, {"a","b","c","d","e"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 4, {"a","b","c","d"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 3, {"a","b","c"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 2, {"a","b"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 1, {"a"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 0, 0, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 1, 4, {"b","c","d","e"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 2, 4, {"c","d","e","f"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 3, 4, {"d","e","f","g"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 4, 4, {"e","f","g","h"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 5, 4, {"f","g","h","i"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 6, 4, {"g","h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 7, 4, {"h","i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 8, 4, {"i","j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 9, 4, {"j"}},
			{{"a","b","c","d","e","f","g","h","i","j"}, 10, 4, {}},
			{{"a","b","c","d","e","f","g","h","i","j"}, tc::io::Path::npos, tc::io::Path::npos, {}},
		};

		for (auto testItr = tests.begin(); testItr != tests.end(); testItr++)
		{
			tc::io::Path::iterator subPathBeginItr = testItr->path.end(), subPathEndItr = testItr->path.end();

			size_t pos = 0;
			for (auto pathItr = testItr->path.begin(); pathItr != testItr->path.end(); pathItr++)
			{
				if (pos == testItr->pos)
					subPathBeginItr = pathItr;
				if (pos == (testItr->pos + testItr->len))
					subPathEndItr = pathItr;

				pos++;
			}

			tc::io::Path subpath = testItr->path.subpath(subPathBeginItr, subPathEndItr);

			if (subpath != testItr->exp_subpath)
			{
				throw tc::Exception(fmt::format(".subpath(begin,end) note({},{}) returned incorrect sub path (returned {}, expected {})", testItr->pos, testItr->len, std::list<std::string>(subpath.begin(), subpath.end()), std::list<std::string>(testItr->exp_subpath.begin(), testItr->exp_subpath.end())));
			}
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Method_to_string()
{
	fmt::print("[tc::io::Path] test_Method_to_string : ");
	try
	{
		struct Test
		{
			tc::io::Path path;
			std::string exp_posix_str;
			std::string exp_win32_str;
		};
		std::vector<Test> tests {
			{{}, "", ""}, // empty path
			{{""}, "/", ""}, // POSIX root path
			{{"C:"}, "C:", "C:\\"}, // windows root path
			{{"c:"}, "c:", "c:\\"}, // windows root path
			{{"data:"}, "data:", "data:"}, // this is a counter test to test the special case handling root paths in POSIX and Win32 formatting
			{{"some", "relative", "path", "that", "has", "seven", "elements"}, "some/relative/path/that/has/seven/elements", "some\\relative\\path\\that\\has\\seven\\elements"}, // relative path
			{{"a dir", "a subdir"}, "a dir/a subdir", "a dir\\a subdir"}, // shorter relative path with spaces
			{{"", "usr", "bin", "bash"}, "/usr/bin/bash", "\\usr\\bin\\bash"}, // absoulute POSIX path
			{{"C:", "Users", "TestUser", "Desktop", "hi.txt"}, "C:/Users/TestUser/Desktop/hi.txt", "C:\\Users\\TestUser\\Desktop\\hi.txt"}, // absoulute Win32 path
			{{std::string(u8"示例文本"), std::string(u8"サンプルテキスト"), std::string(u8"δείγμα κειμένου"), std::string(u8"טקסט לדוגמה")}, std::string(u8"示例文本/サンプルテキスト/δείγμα κειμένου/טקסט לדוגמה"), std::string(u8"示例文本\\サンプルテキスト\\δείγμα κειμένου\\טקסט לדוגמה")} // non-ASCII characters (chinese, japanese, greek, hebrew)
		};

		for (auto itr = tests.begin(); itr != tests.end(); itr++)
		{
#ifdef _WIN32
			std::string exp_native_str = itr->exp_win32_str;
#else
			std::string exp_native_str = itr->exp_posix_str;
#endif

			std::string native_str = itr->path.to_string(tc::io::Path::Format::Native);
			std::string posix_str = itr->path.to_string(tc::io::Path::Format::POSIX);
			std::string win32_str = itr->path.to_string(tc::io::Path::Format::Win32);

			if (native_str != exp_native_str)
			{
				throw tc::Exception(fmt::format(".to_string(Format::Native) failed to format path as string. (returned: \"{}\", expected: \"{}\")", native_str, exp_native_str));
			}

			if (posix_str != itr->exp_posix_str)
			{
				throw tc::Exception(fmt::format(".to_string(Format::POSIX) failed to format path as string. (returned: \"{}\", expected: \"{}\")", posix_str, itr->exp_posix_str));
			}

			if (win32_str != itr->exp_win32_str)
			{
				throw tc::Exception(fmt::format(".to_string(Format::Win32) failed to format path as string. (returned: \"{}\", expected: \"{}\")", win32_str, itr->exp_win32_str));
			}
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Method_to_u16string()
{
	fmt::print("[tc::io::Path] test_Method_to_u16string : ");
	try
	{
		struct Test
		{
			tc::io::Path path;
			std::u16string exp_posix_str;
			std::u16string exp_win32_str;
		};
		std::vector<Test> tests {
			{{}, u"", u""}, // empty path
			{{""}, u"/", u""}, // POSIX root path
			{{"C:"}, u"C:", u"C:\\"}, // windows root path
			{{"data:"}, u"data:", u"data:"}, // this is a counter test to test the special case handling root paths in POSIX and Win32 formatting
			{{"some", "relative", "path", "that", "has", "seven", "elements"}, u"some/relative/path/that/has/seven/elements", u"some\\relative\\path\\that\\has\\seven\\elements"}, // relative path
			{{"a dir", "a subdir"}, u"a dir/a subdir", u"a dir\\a subdir"}, // shorter relative path with spaces
			{{"", "usr", "bin", "bash"}, u"/usr/bin/bash", u"\\usr\\bin\\bash"}, // absoulute POSIX path
			{{"C:", "Users", "TestUser", "Desktop", "hi.txt"}, u"C:/Users/TestUser/Desktop/hi.txt", u"C:\\Users\\TestUser\\Desktop\\hi.txt"}, // absoulute Win32 path
			{{std::string(u8"示例文本"), std::string(u8"サンプルテキスト"), std::string(u8"δείγμα κειμένου"), std::string(u8"טקסט לדוגמה")}, std::u16string(u"示例文本/サンプルテキスト/δείγμα κειμένου/טקסט לדוגמה"), std::u16string(u"示例文本\\サンプルテキスト\\δείγμα κειμένου\\טקסט לדוגמה")} // non-ASCII characters (chinese, japanese, greek, hebrew)
		};

		for (auto itr = tests.begin(); itr != tests.end(); itr++)
		{
#ifdef _WIN32
			std::u16string exp_native_str = itr->exp_win32_str;
#else
			std::u16string exp_native_str = itr->exp_posix_str;
#endif

			std::u16string native_str = itr->path.to_u16string(tc::io::Path::Format::Native);
			std::u16string posix_str = itr->path.to_u16string(tc::io::Path::Format::POSIX);
			std::u16string win32_str = itr->path.to_u16string(tc::io::Path::Format::Win32);

			if (native_str != exp_native_str)
			{
				std::string u8_path_str, u8_exp_path_str;
				tc::string::TranscodeUtil::UTF16ToUTF8(native_str, u8_path_str);
				tc::string::TranscodeUtil::UTF16ToUTF8(exp_native_str, u8_exp_path_str);
				throw tc::Exception(fmt::format(".to_u16string(Format::Native) failed to format path as u16string. (returned: \"{}\", expected: \"{}\")", u8_path_str, u8_exp_path_str));
			}

			if (posix_str != itr->exp_posix_str)
			{
				std::string u8_path_str, u8_exp_path_str;
				tc::string::TranscodeUtil::UTF16ToUTF8(posix_str, u8_path_str);
				tc::string::TranscodeUtil::UTF16ToUTF8(itr->exp_posix_str, u8_exp_path_str);
				throw tc::Exception(fmt::format(".to_u16string(Format::POSIX) failed to format path as u16string. (returned: \"{}\", expected: \"{}\")", u8_path_str, u8_exp_path_str));
			}

			if (win32_str != itr->exp_win32_str)
			{
				std::string u8_path_str, u8_exp_path_str;
				tc::string::TranscodeUtil::UTF16ToUTF8(win32_str, u8_path_str);
				tc::string::TranscodeUtil::UTF16ToUTF8(itr->exp_win32_str, u8_exp_path_str);
				throw tc::Exception(fmt::format(".to_u16string(Format::Win32) failed to format path as u16string. (returned: \"{}\", expected: \"{}\")", u8_path_str, u8_exp_path_str));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Method_to_u32string()
{
	fmt::print("[tc::io::Path] test_Method_to_u32string : ");
	try
	{
		struct Test
		{
			tc::io::Path path;
			std::u32string exp_posix_str;
			std::u32string exp_win32_str;
		};
		std::vector<Test> tests {
			{{}, U"", U""}, // empty path
			{{""}, U"/", U""}, // POSIX root path
			{{"C:"}, U"C:", U"C:\\"}, // windows root path
			{{"data:"}, U"data:", U"data:"}, // this is a counter test to test the special case handling root paths in POSIX and Win32 formatting
			{{"some", "relative", "path", "that", "has", "seven", "elements"}, U"some/relative/path/that/has/seven/elements", U"some\\relative\\path\\that\\has\\seven\\elements"}, // relative path
			{{"a dir", "a subdir"}, U"a dir/a subdir", U"a dir\\a subdir"}, // shorter relative path with spaces
			{{"", "usr", "bin", "bash"}, U"/usr/bin/bash", U"\\usr\\bin\\bash"}, // absoulute POSIX path
			{{"C:", "Users", "TestUser", "Desktop", "hi.txt"}, U"C:/Users/TestUser/Desktop/hi.txt", U"C:\\Users\\TestUser\\Desktop\\hi.txt"}, // absoulute Win32 path
			{{std::string(u8"示例文本"), std::string(u8"サンプルテキスト"), std::string(u8"δείγμα κειμένου"), std::string(u8"טקסט לדוגמה")}, std::u32string(U"示例文本/サンプルテキスト/δείγμα κειμένου/טקסט לדוגמה"), std::u32string(U"示例文本\\サンプルテキスト\\δείγμα κειμένου\\טקסט לדוגמה")} // non-ASCII characters (chinese, japanese, greek, hebrew)
		};

		for (auto itr = tests.begin(); itr != tests.end(); itr++)
		{
#ifdef _WIN32
			std::u32string exp_native_str = itr->exp_win32_str;
#else
			std::u32string exp_native_str = itr->exp_posix_str;
#endif

			std::u32string native_str = itr->path.to_u32string(tc::io::Path::Format::Native);
			std::u32string posix_str = itr->path.to_u32string(tc::io::Path::Format::POSIX);
			std::u32string win32_str = itr->path.to_u32string(tc::io::Path::Format::Win32);

			if (native_str != exp_native_str)
			{
				std::string u8_path_str, u8_exp_path_str;
				tc::string::TranscodeUtil::UTF32ToUTF8(native_str, u8_path_str);
				tc::string::TranscodeUtil::UTF32ToUTF8(exp_native_str, u8_exp_path_str);
				throw tc::Exception(fmt::format(".to_u32string(Format::Native) failed to format path as u32string. (returned: \"{}\", expected: \"{}\")", u8_path_str, u8_exp_path_str));
			}

			if (posix_str != itr->exp_posix_str)
			{
				std::string u8_path_str, u8_exp_path_str;
				tc::string::TranscodeUtil::UTF32ToUTF8(posix_str, u8_path_str);
				tc::string::TranscodeUtil::UTF32ToUTF8(itr->exp_posix_str, u8_exp_path_str);
				throw tc::Exception(fmt::format(".to_u32string(Format::POSIX) failed to format path as u32string. (returned: \"{}\", expected: \"{}\")", u8_path_str, u8_exp_path_str));
			}

			if (win32_str != itr->exp_win32_str)
			{
				std::string u8_path_str, u8_exp_path_str;
				tc::string::TranscodeUtil::UTF32ToUTF8(win32_str, u8_path_str);
				tc::string::TranscodeUtil::UTF32ToUTF8(itr->exp_win32_str, u8_exp_path_str);
				throw tc::Exception(fmt::format(".to_u32string(Format::Win32) failed to format path as u32string. (returned: \"{}\", expected: \"{}\")", u8_path_str, u8_exp_path_str));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Operator_string()
{
	fmt::print("[tc::io::Path] test_Operator_string : ");
	try
	{
		struct Test
		{
			tc::io::Path path;
			std::string exp_posix_str;
			std::string exp_win32_str;
		};
		std::vector<Test> tests {
			{{}, "", ""}, // empty path
			{{""}, "/", ""}, // POSIX root path
			{{"C:"}, "C:", "C:\\"}, // windows root path
			{{"c:"}, "c:", "c:\\"}, // windows root path
			{{"data:"}, "data:", "data:"}, // this is a counter test to test the special case handling root paths in POSIX and Win32 formatting
			{{"some", "relative", "path", "that", "has", "seven", "elements"}, "some/relative/path/that/has/seven/elements", "some\\relative\\path\\that\\has\\seven\\elements"}, // relative path
			{{"a dir", "a subdir"}, "a dir/a subdir", "a dir\\a subdir"}, // shorter relative path with spaces
			{{"", "usr", "bin", "bash"}, "/usr/bin/bash", "\\usr\\bin\\bash"}, // absoulute POSIX path
			{{"C:", "Users", "TestUser", "Desktop", "hi.txt"}, "C:/Users/TestUser/Desktop/hi.txt", "C:\\Users\\TestUser\\Desktop\\hi.txt"}, // absoulute Win32 path
			{{std::string(u8"示例文本"), std::string(u8"サンプルテキスト"), std::string(u8"δείγμα κειμένου"), std::string(u8"טקסט לדוגמה")}, std::string(u8"示例文本/サンプルテキスト/δείγμα κειμένου/טקסט לדוגמה"), std::string(u8"示例文本\\サンプルテキスト\\δείγμα κειμένου\\טקסט לדוגמה")} // non-ASCII characters (chinese, japanese, greek, hebrew)
		};

		for (auto itr = tests.begin(); itr != tests.end(); itr++)
		{
#ifdef _WIN32
			std::string exp_native_str = itr->exp_win32_str;
#else
			std::string exp_native_str = itr->exp_posix_str;
#endif

			std::string native_str = itr->path;

			if (native_str != exp_native_str)
			{
				throw tc::Exception(fmt::format("operator string() failed to format path as string. (returned: \"{}\", expected: \"{}\")", native_str, exp_native_str));
			}
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Operator_u16string()
{
	fmt::print("[tc::io::Path] test_Operator_u16string : ");
	try
	{
		struct Test
		{
			tc::io::Path path;
			std::u16string exp_posix_str;
			std::u16string exp_win32_str;
		};
		std::vector<Test> tests {
			{{}, u"", u""}, // empty path
			{{""}, u"/", u""}, // POSIX root path
			{{"C:"}, u"C:", u"C:\\"}, // windows root path
			{{"data:"}, u"data:", u"data:"}, // this is a counter test to test the special case handling root paths in POSIX and Win32 formatting
			{{"some", "relative", "path", "that", "has", "seven", "elements"}, u"some/relative/path/that/has/seven/elements", u"some\\relative\\path\\that\\has\\seven\\elements"}, // relative path
			{{"a dir", "a subdir"}, u"a dir/a subdir", u"a dir\\a subdir"}, // shorter relative path with spaces
			{{"", "usr", "bin", "bash"}, u"/usr/bin/bash", u"\\usr\\bin\\bash"}, // absoulute POSIX path
			{{"C:", "Users", "TestUser", "Desktop", "hi.txt"}, u"C:/Users/TestUser/Desktop/hi.txt", u"C:\\Users\\TestUser\\Desktop\\hi.txt"}, // absoulute Win32 path
			{{std::string(u8"示例文本"), std::string(u8"サンプルテキスト"), std::string(u8"δείγμα κειμένου"), std::string(u8"טקסט לדוגמה")}, std::u16string(u"示例文本/サンプルテキスト/δείγμα κειμένου/טקסט לדוגמה"), std::u16string(u"示例文本\\サンプルテキスト\\δείγμα κειμένου\\טקסט לדוגמה")} // non-ASCII characters (chinese, japanese, greek, hebrew)
		};

		for (auto itr = tests.begin(); itr != tests.end(); itr++)
		{
#ifdef _WIN32
			std::u16string exp_native_str = itr->exp_win32_str;
#else
			std::u16string exp_native_str = itr->exp_posix_str;
#endif

			std::u16string native_str = itr->path;

			if (native_str != exp_native_str)
			{
				std::string u8_path_str, u8_exp_path_str;
				tc::string::TranscodeUtil::UTF16ToUTF8(native_str, u8_path_str);
				tc::string::TranscodeUtil::UTF16ToUTF8(exp_native_str, u8_exp_path_str);
				throw tc::Exception(fmt::format("operator u16string() failed to format path as u16string. (returned: \"{}\", expected: \"{}\")", u8_path_str, u8_exp_path_str));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Operator_u32string()
{
	fmt::print("[tc::io::Path] test_Operator_u32string : ");
	try
	{
		struct Test
		{
			tc::io::Path path;
			std::u32string exp_posix_str;
			std::u32string exp_win32_str;
		};
		std::vector<Test> tests {
			{{}, U"", U""}, // empty path
			{{""}, U"/", U""}, // POSIX root path
			{{"C:"}, U"C:", U"C:\\"}, // windows root path
			{{"data:"}, U"data:", U"data:"}, // this is a counter test to test the special case handling root paths in POSIX and Win32 formatting
			{{"some", "relative", "path", "that", "has", "seven", "elements"}, U"some/relative/path/that/has/seven/elements", U"some\\relative\\path\\that\\has\\seven\\elements"}, // relative path
			{{"a dir", "a subdir"}, U"a dir/a subdir", U"a dir\\a subdir"}, // shorter relative path with spaces
			{{"", "usr", "bin", "bash"}, U"/usr/bin/bash", U"\\usr\\bin\\bash"}, // absoulute POSIX path
			{{"C:", "Users", "TestUser", "Desktop", "hi.txt"}, U"C:/Users/TestUser/Desktop/hi.txt", U"C:\\Users\\TestUser\\Desktop\\hi.txt"}, // absoulute Win32 path
			{{std::string(u8"示例文本"), std::string(u8"サンプルテキスト"), std::string(u8"δείγμα κειμένου"), std::string(u8"טקסט לדוגמה")}, std::u32string(U"示例文本/サンプルテキスト/δείγμα κειμένου/טקסט לדוגמה"), std::u32string(U"示例文本\\サンプルテキスト\\δείγμα κειμένου\\טקסט לדוגמה")} // non-ASCII characters (chinese, japanese, greek, hebrew)
		};

		for (auto itr = tests.begin(); itr != tests.end(); itr++)
		{
#ifdef _WIN32
			std::u32string exp_native_str = itr->exp_win32_str;
#else
			std::u32string exp_native_str = itr->exp_posix_str;
#endif

			std::u32string native_str = itr->path;

			if (native_str != exp_native_str)
			{
				std::string u8_path_str, u8_exp_path_str;
				tc::string::TranscodeUtil::UTF32ToUTF8(native_str, u8_path_str);
				tc::string::TranscodeUtil::UTF32ToUTF8(exp_native_str, u8_exp_path_str);
				throw tc::Exception(fmt::format("operator u32string() failed to format path as u32string. (returned: \"{}\", expected: \"{}\")", u8_path_str, u8_exp_path_str));
			}
		}
		
		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Operator_Addition()
{
	fmt::print("[tc::io::Path] test_Operator_Addition : ");
	try
	{
		const std::string raw_path_a = "foo/bar/";
		const std::string raw_path_b = "file.txt";
		const std::string raw_path_ab = raw_path_a + raw_path_b;

		tc::io::Path path_a(raw_path_a);
		tc::io::Path path_b(raw_path_b);
		tc::io::Path path_ab = path_a + path_b;

		std::string test_path;
		tc::io::PathUtil::pathToUnixUTF8(path_ab, test_path);

		if (test_path != raw_path_ab)
		{
			throw tc::Exception("operator+() did not operate produce expected output");
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Operator_Append()
{
	fmt::print("[tc::io::Path] test_Operator_Append : ");
	try
	{
		const std::string raw_path_a = "foo/bar/";
		const std::string raw_path_b = "file.txt";
		const std::string raw_path_ab = raw_path_a + raw_path_b;

		tc::io::Path path_a(raw_path_a);
		tc::io::Path path_b(raw_path_b);
		path_a += path_b;

		std::string test_path;
		tc::io::PathUtil::pathToUnixUTF8(path_a, test_path);

		if (test_path != raw_path_ab)
		{
			throw tc::Exception("operator+() did not operate produce expected output");
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Scenario_AppendStressTest()
{
	fmt::print("[tc::io::Path] test_Scenario_AppendStressTest : ");
	try
	{
		const std::string raw_dir_path = "foo/bar/";
		const std::string raw_file_path = "file.txt";
		size_t append_itteration_count = 1000;
		size_t expected_element_count = (append_itteration_count * 2) + 1;
	
		tc::io::Path path;
		tc::io::Path path_dir(raw_dir_path);

		for (size_t i = 0; i < append_itteration_count; i++)
			path += path_dir;

		path += tc::io::Path(raw_file_path);

		if (path.size() != expected_element_count)
		{
			throw tc::Exception("unexpected number of path elements");
		}

		if (path.empty() != (expected_element_count == 0))
		{
			throw tc::Exception("Path did not have expected element count (empty() returned unexpected value.)");
		}

		tc::io::Path::const_iterator itr = path.end();
		if (*(--itr) != "file.txt")
		{
			throw tc::Exception("Unexpected value for tested path element");
		}

		if (*(--itr) != "bar")
		{
			throw tc::Exception("Unexpected value for tested path element");
		}

		if (*(--itr) != "foo")
		{
			throw tc::Exception("Unexpected value for tested path element");
		}

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Operator_EqualityTest()
{
	fmt::print("[tc::io::Path] test_Operator_EqualityTest : ");
	try
	{
		std::string raw_path_0 = "a directory/a subdirectory";

		tc::io::Path path_a(raw_path_0);
		tc::io::Path path_b(raw_path_0);

		if ((path_a == path_b) == false)
			throw tc::Exception("operator==() did not return true for equal Path objects");

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Operator_InequalityTest()
{
	fmt::print("[tc::io::Path] test_Operator_InequalityTest : ");
	try
	{
		std::string raw_path_0 = "a directory/a subdirectory";
		std::string raw_path_1 = "a different directory/a different subdirectory";
		tc::io::Path path_a(raw_path_0);
		tc::io::Path path_b(raw_path_1);

		if ((path_a != path_b) == false)
			throw tc::Exception("operator!=() did not return true for unequal Path objects");

		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}

void io_Path_TestClass::test_Operator_LessThanTest()
{
	fmt::print("[tc::io::Path] test_Operator_LessThanTest : ");
	try
	{
		std::string raw_path_a = "/a/path/to/check/";
		std::string raw_path_b = "/a/path/to/check/that/is/longer";
		std::string raw_path_c = "/a/path/";
		std::string raw_path_d = "/very/different/path/";
		tc::io::Path path_a(raw_path_a);
		tc::io::Path path_a_duplicate(raw_path_a);
		tc::io::Path path_b(raw_path_b);
		tc::io::Path path_c(raw_path_c);
		tc::io::Path path_d(raw_path_d);

		// /a/path/to/check/ is not less than a copy of itself
		if ((path_a < path_a_duplicate) != false)
		{
			throw tc::Exception("operator<() returned true for equal Path objects");
		}

		// /a/path/to/check/ is less than /a/path/to/check/that/is/longer
		if ((path_a < path_b) != true)
		{
			throw tc::Exception("operator<() returned false for Path objects where (" + raw_path_a + ") < (" + raw_path_b + ")");
		}

		// /a/path/to/check/ is not less than /a/path/
		if ((path_a < path_c) != false)
		{
			throw tc::Exception("operator<() returned true for Path objects where (" + raw_path_a + ") < (" + raw_path_c + ")");
		}

		// /a/path/to/check/ is less than /very/different/path/
		if ((path_a < path_d) != true)
		{
			throw tc::Exception("operator<() returned false for Path objects where (" + raw_path_a + ") < (" + raw_path_d + ")");
		}


		fmt::print("PASS\n");
	}
	catch (const tc::Exception& e)
	{
		fmt::print("FAIL ({})\n", e.error());
	}
	catch (const std::exception& e)
	{
		fmt::print("FAIL UNEXPECTED ({})", e.what());
	}
}