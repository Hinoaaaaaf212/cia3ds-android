#pragma once

class ITestClass
{
public:
	virtual ~ITestClass() = default;
	virtual void runAllTests() = 0;
};