#include <tc/Exception.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <tc/io/BasicPathResolver.h>
#include <tc/io/PathUtil.h>

#include "io_BasicPathResolver_TestClass.h"
#include "StreamTestUtil.h"

void io_BasicPathResolver_TestClass::runAllTests(void)
{
	fmt::print("[tc::io::BasicPathResolver] START\n");
	test_EmptyStateAfterDefaultConstructor();
	test_setCurrentDirectory();
	test_setExplicitRootLabels();
	test_ResolveRootDirRelativePaths();
	test_ResolveWorkingDirectoryRelativePaths();
	test_ResolvePathsWithCustomRootLabels();
	fmt::print("[tc::io::BasicPathResolver] END\n");
}

void io_BasicPathResolver_TestClass::test_EmptyStateAfterDefaultConstructor()
{
	fmt::print("[tc::io::BasicPathResolver] test_EmptyStateAfterDefaultConstructor : ");
	try
	{
		try 
		{
			tc::io::BasicPathResolver resolver;

			tc::io::Path cur_dir = resolver.getCurrentDirectory();
			tc::io::Path expected_cur_dir = tc::io::Path("/");
			if (cur_dir != expected_cur_dir)
			{
				throw tc::Exception(fmt::format(".getCurrentDirectory() returned the wrong path after default constructor (returned \"{}\", expected \"{}\")", (std::string)cur_dir, (std::string)expected_cur_dir));
			}

			std::vector<std::string> root_labels = resolver.getExplicitRootLabels();
			std::vector<std::string> expected_root_labels = {};
			if (root_labels != expected_root_labels)
			{
				throw tc::Exception(fmt::format(".getExplicitRootLabels() returned the root label list after default constructor (returned {}, expected {})", root_labels, expected_root_labels));
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

void io_BasicPathResolver_TestClass::test_setCurrentDirectory()
{
	fmt::print("[tc::io::BasicPathResolver] test_setCurrentDirectory : ");
	try
	{
		try 
		{
			tc::io::BasicPathResolver resolver;

			try 
			{
				resolver.setCurrentDirectory(tc::io::Path());
				throw tc::Exception(".setCurrentDirectory() failed to throw tc::ArgumentOutOfRangeException when passed an empty path");
			}
			catch (const tc::ArgumentOutOfRangeException&)
			{
				// do nothing
			}

			tc::io::Path expected_cur_dir = tc::io::Path("a/this/is/a/path/to/set");
			resolver.setCurrentDirectory(expected_cur_dir);
			tc::io::Path cur_dir = resolver.getCurrentDirectory();
		
			if (cur_dir != expected_cur_dir)
			{
				throw tc::Exception(fmt::format(".getCurrentDirectory() returned the wrong path after .setCurrentDirectory() (returned \"{}\", expected \"{}\")", (std::string)cur_dir, (std::string)expected_cur_dir));
			}

			std::vector<std::string> root_labels = resolver.getExplicitRootLabels();
			std::vector<std::string> expected_root_labels = {};
			if (root_labels != expected_root_labels)
			{
				throw tc::Exception(fmt::format(".getExplicitRootLabels() returned unexpected data list after .setCurrentDirectory() (returned {}, expected {})", root_labels, expected_root_labels));
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

void io_BasicPathResolver_TestClass::test_setExplicitRootLabels()
{
	fmt::print("[tc::io::BasicPathResolver] test_setExplicitRootLabels : ");
	try
	{
		try 
		{
			tc::io::BasicPathResolver resolver;

			std::vector<std::string> expected_root_labels = {"C:", "D:", "E:"};
			resolver.setExplicitRootLabels(expected_root_labels);
			std::vector<std::string> root_labels = resolver.getExplicitRootLabels();
			
			if (root_labels != expected_root_labels)
			{
				throw tc::Exception(fmt::format(".getExplicitRootLabels() returned unexpected data list after .setExplicitRootLabels() (returned {}, expected {})", root_labels, expected_root_labels));
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

void io_BasicPathResolver_TestClass::test_ResolveRootDirRelativePaths()
{
	fmt::print("[tc::io::BasicPathResolver] test_ResolveRootDirRelativePaths : ");
	try
	{
		try 
		{
			struct ResolutionTest {
				std::string in_path;
				std::string in_working_directory_path;
				std::string expected_resolved_path;
			};

			std::vector<ResolutionTest> tests = {
				{"/a/path", "/", "/a/path"},
				{"/a/path", "/a/working/directory", "/a/path"},
				{"rom:/a/path", "rom:/a/working/directory", "rom:/a/path"},
				{"/a/path/////with/empty//path/elements/", "/a/working/directory", "/a/path/with/empty/path/elements/"},
				{"/a/path", "/another/working/directory", "/a/path"},
				{"/a/path", "/a/very/long/working/directory/path/indeed/this/is/more/than/ten/elements", "/a/path"},
				{"/a/path/with/../some/../../parent/directory/aliases/../", "/a/working/directory", "/a/parent/directory"},
				{"/a/path/with/./some/././current/directory/aliases/./", "/a/working/directory", "/a/path/with/some/current/directory/aliases"},
				{"/a/path/with/./a/./../mix/./of/../../parent/and/../current/directory/aliases/./", "/a/working/directory", "/a/path/with/parent/current/directory/aliases"},
				{"/a/path/with/../../../more/../../parent/directory/../../../aliases/../than/../path/../elements/../..", "/a/working/directory", "/"},
			};

			for (auto itr = tests.begin(); itr != tests.end(); itr++)
			{
				util_RunResolutionTest(tc::io::Path(itr->in_path), tc::io::Path(itr->in_working_directory_path), tc::io::Path(itr->expected_resolved_path));
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

void io_BasicPathResolver_TestClass::test_ResolveWorkingDirectoryRelativePaths()
{
	fmt::print("[tc::io::BasicPathResolver] test_ResolveWorkingDirectoryRelativePaths : ");
	try
	{
		try 
		{
			struct ResolutionTest {
				std::string in_path;
				std::string in_working_directory_path;
				std::string expected_resolved_path;
			};

			std::vector<ResolutionTest> tests = {
				{"./a/path", "/", "/a/path"},
				{"a/path", "/", "/a/path"},
				{"./a/path", "rom:/", "rom:/a/path"},
				{"a/path", "rom:/", "rom:/a/path"},
				{"/a/path", "rom:/", "rom:/a/path"},
				{"./a/path", "/a/working/directory", "/a/working/directory/a/path"},
				{"a/path", "/a/working/directory", "/a/working/directory/a/path"},
				{"./a/path", "/a/very/long/working/directory/path/indeed/this/is/more/than/ten/elements", "/a/very/long/working/directory/path/indeed/this/is/more/than/ten/elements/a/path"},
				{"a/path", "/a/very/long/working/directory/path/indeed/this/is/more/than/ten/elements", "/a/very/long/working/directory/path/indeed/this/is/more/than/ten/elements/a/path"},
				{"./a/path/with/../some/../../parent/directory/aliases/../", "/a/working/directory", "/a/working/directory/a/parent/directory"},
				{"a/path/with/../some/../../parent/directory/aliases/../", "/a/working/directory", "/a/working/directory/a/parent/directory"},
				{"./a/path/with/./a/./../mix/./of/../../parent/and/../current/directory/aliases/./", "/a/working/directory", "/a/working/directory/a/path/with/parent/current/directory/aliases/"},
				{"a/path/with/./a/./../mix/./of/../../parent/and/../current/directory/aliases/./", "/a/working/directory", "/a/working/directory/a/path/with/parent/current/directory/aliases/"},
				{"./a/path/with/../../../more/../../parent/directory/../../../aliases/../than/../path/../elements/../../very/different/path", "/a/working/directory", "/very/different/path"},
				{"a/path/with/../../../more/../../parent/directory/../../../aliases/../than/../path/../elements/../../very/different/path", "/a/working/directory", "/very/different/path"},

			};

			for (auto itr = tests.begin(); itr != tests.end(); itr++)
			{
				util_RunResolutionTest(tc::io::Path(itr->in_path), tc::io::Path(itr->in_working_directory_path), tc::io::Path(itr->expected_resolved_path));
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

void io_BasicPathResolver_TestClass::test_ResolvePathsWithCustomRootLabels()
{
	fmt::print("[tc::io::BasicPathResolver] test_ResolvePathsWithCustomRootLabels : ");
	try
	{
		try 
		{
			struct ResolutionTest {
				std::string in_path;
				std::string in_working_directory_path;
				std::vector<std::string> root_labels;
				std::string expected_resolved_path;
			};

			std::vector<ResolutionTest> tests = {
				{"Project\\MyFile.txt", "C:\\Users\\Administrator\\Desktop", {"C:", "D:"}, "C:\\Users\\Administrator\\Desktop\\Project\\MyFile.txt"},
				{"C:\\Source\\Project\\MyFile.txt", "C:\\Users\\Administrator\\Desktop", {"C:", "D:"}, "C:\\Source\\Project\\MyFile.txt"},
				{"D:\\Project\\MyFile.txt", "C:\\Users\\Administrator\\Desktop", {"C:", "D:"}, "D:\\Project\\MyFile.txt"},
				{"E:\\Project\\MyFile.txt", "C:\\Users\\Administrator\\Desktop", {"C:", "D:"}, "C:\\Users\\Administrator\\Desktop\\E:\\Project\\MyFile.txt"},	
			};

			for (auto itr = tests.begin(); itr != tests.end(); itr++)
			{
				util_RunResolutionTest(tc::io::Path(itr->in_path), tc::io::Path(itr->in_working_directory_path), itr->root_labels, tc::io::Path(itr->expected_resolved_path));
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

void io_BasicPathResolver_TestClass::util_RunResolutionTest(const tc::io::Path& in_path, const tc::io::Path& in_working_dir_path, const tc::io::Path& expected_resolved_path)
{
	util_RunResolutionTest(in_path, in_working_dir_path, {}, expected_resolved_path);
}

void io_BasicPathResolver_TestClass::util_RunResolutionTest(const tc::io::Path& in_path, const tc::io::Path& in_working_dir_path, const std::vector<std::string>& root_labels, const tc::io::Path& expected_resolved_path)
{
	tc::io::BasicPathResolver res;

	res.setCurrentDirectory(in_working_dir_path);
	res.setExplicitRootLabels(root_labels);

	tc::io::Path resolved_path = res.resolveCanonicalPath(in_path);

	if (resolved_path != expected_resolved_path)
	{
		throw tc::Exception(fmt::format("Resolve (path=\"{:s}\", workingdir=\"{:s}\", rootlabels={}) returned \"{:s}\"  (expected: \"{:s}\").", (std::string)in_path, (std::string)in_working_dir_path, root_labels, (std::string)resolved_path, (std::string)expected_resolved_path));
	}
}