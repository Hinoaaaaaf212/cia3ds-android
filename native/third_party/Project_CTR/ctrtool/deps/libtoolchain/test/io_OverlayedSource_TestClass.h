#pragma once
#include "ITestClass.h"

#include <tc/io/OverlayedSource.h>

class io_OverlayedSource_TestClass : public ITestClass
{
public:
	void runAllTests();
private:
	void testDefaultConstructor();
	void testSingleOverlayConstructor();
	void testMultiOverlayConstructor();
	void testNullBaseStream();
	void testNullOverlayStream();
	void testOverlayStreamTooSmallForOverlayRegion();
	void testOverlayRegionBeforeBaseStream();
	void testOverlayRegionPartlyBeforeBaseStream();
	void testOverlayRegionAfterBaseStream();
	void testOverlayRegionPartlyAfterBaseStream();
};
