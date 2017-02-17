#ifndef ofAppWindowMock_h
#define ofAppWindowMock_h

#include "ofAppNoWindow.h"

class ofAppNoWindowAlwaysClose : public ofAppNoWindow {
	virtual bool getWindowShouldClose() {
		return true;
	}
};

#endif
