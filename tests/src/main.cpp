// Tests assume that ofxApp/tests is the working directory

#include <stdio.h>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ofMain.h"
#include "ofAppNoWindow.h"


// The following should be defined during the build process
//
// OFXAPP_TESTING
// TARGET_NODISPLAY
// USE_OFX_FONTSTASH
// USE_OFX_HISTORYPLOT


GTEST_API_ int main(int argc, char **argv) {
	printf("Running ofxApp test suite\n");
	::testing::InitGoogleMock(&argc, argv);
	
	// All tests should run in a constant, reproducable environment. However,
	// OpenFrameworks has a lot of assumptions about there being one global environment
	// for the entire application's life span. So for this purpose, let's create that
	// environment here so we can make sure every test is executing with the same base.
	// (any changes to this global environments state during a test, however, *must* be cleaned
	// up in that test, to ensure everybody get's the same starting condition).
	//
	// Note that we're not using ofCreateWindow. Doing so would mean we would need to rebuild
	// OpenFrameworks with the TARGET_NODISPLAY macro, which we currently are not doing for
	// Travis-CI. Therefore, we do the steps ourselves to make sure that ofAppNoWindow actually
	// gets created
	ofInit();
	shared_ptr<ofAppNoWindow> window = shared_ptr<ofAppNoWindow>(new ofAppNoWindow());
	ofGetMainLoop()->addWindow(window);
	window->setup(ofWindowSettings());
	
	return RUN_ALL_TESTS();
}
