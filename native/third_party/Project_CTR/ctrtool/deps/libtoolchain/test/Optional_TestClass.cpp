#include <tc/Exception.h>
#include <iostream>

#include "Optional_TestClass.h"

void Optional_TestClass::runAllTests()
{
	std::cout << "[tc::Optional] START" << std::endl;
	testDefaultConstructor();
	testWrapConstructor();
	testCopyConstructorFromNullOptional();
	testCopyConstructorFromExistingOptional();
	testWrapOperator();
	testCopyOperatorFromNullOptional();
	testCopyOperatorFromExistingOptional();
	testMakeNullOnNullOptional();
	testMakeNullOnExistingOptional();
	std::cout << "[tc::Optional] END" << std::endl;
}

void Optional_TestClass::testDefaultConstructor()
{
	std::cout << "[tc::Optional] testDefaultConstructor : " << std::flush;
	try
	{
		{
			tc::Optional<int> foo;

			if (foo.isNull() == false)
			{
				throw tc::Exception("Default constructor created an object with null state, but isNull() returned false");
			}

			if (foo.isSet() == true)
			{
				throw tc::Exception("Default constructor created an object with null state, but isSet() returned true");
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "FAIL (" << e.what() << ")" << std::endl;
	}
	
}

void Optional_TestClass::testWrapConstructor()
{
	std::cout << "[tc::Optional] testWrapConstructor : " << std::flush;
	try
	{
		{
			int testInt = 42;

			tc::Optional<int> foo(testInt);

			if (foo.isNull() == true)
			{
				throw tc::Exception("Wrapping constructor created an object with a valid state, but isNull() returned true");
			}

			if (foo.isSet() == false)
			{
				throw tc::Exception("Wrapping constructor created an object with a valid state, but isSet() returned false");
			}

			if (foo.get() != testInt)
			{
				throw tc::Exception("Wrapping constructor created an object with an incorrect value");
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "FAIL (" << e.what() << ")" << std::endl;
	}
}

void Optional_TestClass::testCopyConstructorFromNullOptional()
{
	std::cout << "[tc::Optional] testCopyConstructorFromNullOptional : " << std::flush;
	try
	{
		{
			tc::Optional<int> foo;
			tc::Optional<int> bar(foo);

			if (bar.isNull() == false)
			{
				throw tc::Exception("Copy constructor created an object with a null state, but isNull() returned false");
			}

			if (bar.isSet() == true)
			{
				throw tc::Exception("Copy constructor created an object with a null state, but isSet() returned true");
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "FAIL (" << e.what() << ")" << std::endl;
	}
}

void Optional_TestClass::testCopyConstructorFromExistingOptional()
{
	std::cout << "[tc::Optional] testCopyConstructorFromExistingOptional : " << std::flush;
	try
	{
		{
			int testInt = 42;

			tc::Optional<int> foo(testInt);
			tc::Optional<int> bar(foo);

			if (bar.isNull() == true)
			{
				throw tc::Exception("Copy constructor created an object with a set state, but isNull() returned true");
			}
			if (bar.isSet() == false)
			{
				throw tc::Exception("Copy constructor created an object with a set state, but isSet() returned true");
			}

			if (bar.get() != testInt)
			{
				throw tc::Exception("Copy constructor created an object where the wrapped value was unexpected");
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "FAIL (" << e.what() << ")" << std::endl;
	}
}

void Optional_TestClass::testWrapOperator()
{
	std::cout << "[tc::Optional] testWrapOperator : " << std::flush;
	try
	{
		{
			int testInt = 42;

			tc::Optional<int> foo;

			foo = testInt;

			if (foo.isNull() == true)
			{
				throw tc::Exception("Wrap operator created an object with a set state, but isNull() returned true");
			}
			if (foo.isSet() == false)
			{
				throw tc::Exception("Wrap operator created an object with a set state, but isSet() returned false");
			}

			if (foo.get() != testInt)
			{
				throw tc::Exception("Wrap operator created an object where the wrapped value was unexpected");
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "FAIL (" << e.what() << ")" << std::endl;
	}
}

void Optional_TestClass::testCopyOperatorFromNullOptional()
{
	std::cout << "[tc::Optional] testCopyOperatorFromNullOptional : " << std::flush;
	try
	{
		{
			tc::Optional<int> foo;
			tc::Optional<int> bar  = foo;

			if (bar.isNull() == false)
			{
				throw tc::Exception("Copy operator created an object with a null state, but isNull() returned false");
			}

			if (bar.isSet() == true)
			{
				throw tc::Exception("Copy operator created an object with a null state, but isSet() returned false");
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "FAIL (" << e.what() << ")" << std::endl;
	}
}

void Optional_TestClass::testCopyOperatorFromExistingOptional()
{
	std::cout << "[tc::Optional] testCopyOperatorFromExistingOptional : " << std::flush;
	try
	{
		{
			int testInt = 42;

			tc::Optional<int> foo(testInt);
			tc::Optional<int> bar = foo;

			if (bar.isNull() == true)
			{
				throw tc::Exception("Copy operator created an object with a set state, but isNull() returned true");
			}
			if (bar.isSet() == false)
			{
				throw tc::Exception("Copy operator created an object with a set state, but isSet() returned false");
			}

			if (bar.get() != testInt)
			{
				throw tc::Exception("Copy operator created an object where the wrapped value was unexpected");
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "FAIL (" << e.what() << ")" << std::endl;
	}
}

void Optional_TestClass::testMakeNullOnNullOptional()
{
	std::cout << "[tc::Optional] testMakeNullOnNullOptional : " << std::flush;
	try
	{
		{
			tc::Optional<int> foo;

			foo.makeNull();

			if (foo.isNull() == false)
			{
				throw tc::Exception("A null Object was nulled by makeNull(), but isNull() returned false");
			}

			if (foo.isSet() == true)
			{
				throw tc::Exception("A null Object was nulled by makeNull() but isSet() returned true");
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "FAIL (" << e.what() << ")" << std::endl;
	}
}

void Optional_TestClass::testMakeNullOnExistingOptional()
{
	std::cout << "[tc::Optional] testMakeNullOnExistingOptional : " << std::flush;
	try
	{
		{
			int testInt = 42;

			tc::Optional<int> foo(testInt);

			foo.makeNull();

			if (foo.isNull() == false)
			{
				throw tc::Exception("A set Object was nulled by makeNull(), but isNull() returned false");
			}

			if (foo.isSet() == true)
			{
				throw tc::Exception("A set Object was nulled by makeNull(), but isSet() returned true");
			}
		}

		std::cout << "PASS" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "FAIL (" << e.what() << ")" << std::endl;
	}
}