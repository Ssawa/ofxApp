#ifndef ofxAppDelegateMock_h
#define ofxAppDelegateMock_h

#include "gmock/gmock.h"

#include "ofxAppDelegate.h"

class MockDelegate : public ofxAppDelegate {
public:
	MOCK_METHOD1(ofxAppPhaseWillBegin, void(ofxApp::Phase phase));
	MOCK_METHOD1(ofxAppIsPhaseComplete, bool(ofxApp::Phase phase));
	
	MOCK_METHOD1(ofxAppDrawPhaseProgress, void(ofxApp::Phase phase));
	MOCK_METHOD1(ofxAppGetStatusString, string(ofxApp::Phase phase));
	MOCK_METHOD1(ofxAppGetProgressForPhase, float(ofxApp::Phase phase));
	MOCK_METHOD2(ofxAppContentIsReady, void(const string & contentID, vector<ContentObject*>));
	
	MOCK_METHOD1(tuioAdded, void(ofxTuioCursor & tuioCursor));
	MOCK_METHOD1(tuioRemoved, void(ofxTuioCursor & tuioCursor));
	MOCK_METHOD1(tuioUpdated, void(ofxTuioCursor & tuioCursor));
	
	MOCK_METHOD1(screenSetupChanged, void(ofxScreenSetup::ScreenSetupArg &arg));
};

#endif
