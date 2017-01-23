//
//  ofxAppStructs.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 3/8/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxAppMacros.h"
#include "ofxMtJsonParser.h"

class ContentObject;

namespace ofxApp{

	//user created lambdas to do custom actions at the crucial points
	struct ParseFunctions{
		std::function<void (ofxMtJsonParserThread::JsonStructureData &)> pointToObjects;
		std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseOneObject;
		//std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> defineObjectAssets;
		std::function<void (ContentObject*)> setupTexturedObject;
		ofxJSON userData;
	};

	enum class State : int{ //internal states used by an ofxApp app
		SETUP_OFXAPP_INTERNALS,
		SETUP_DELEGATE_B4_CONTENT_LOAD,
		LOAD_STATIC_TEXTURES,
		LOAD_JSON_CONTENT,
		LOAD_JSON_CONTENT_FAILED,
		DELIVER_CONTENT_LOAD_RESULTS,
		SETUP_DELEGATE_B4_RUNNING,
		RUNNING,
	};

	//ofxAppDelegate exposed states; as they are a subset of the internal ofxApp states, enum
	//values are matched to them
	enum class Phase : int{
		WILL_LOAD_CONTENT = (int)State::SETUP_DELEGATE_B4_CONTENT_LOAD,
		DID_DELIVER_CONTENT = (int)State::DELIVER_CONTENT_LOAD_RESULTS,
		WILL_BEGIN_RUNNING = (int)State::SETUP_DELEGATE_B4_RUNNING
	};

	//convenience methods to be able to print state names
	inline string toString(const State & s){
		switch(s){
			case State::SETUP_OFXAPP_INTERNALS: return "SETUP_OFXAPP_INTERNALS";
			case State::SETUP_DELEGATE_B4_CONTENT_LOAD: return "SETUP_DELEGATE_B4_CONTENT_LOAD";
			case State::LOAD_STATIC_TEXTURES: return "LOAD_STATIC_TEXTURES";
			case State::LOAD_JSON_CONTENT: return "LOAD_JSON_CONTENT";
			case State::LOAD_JSON_CONTENT_FAILED: return "LOAD_JSON_CONTENT_FAILED";
			case State::DELIVER_CONTENT_LOAD_RESULTS: return "DELIVER_CONTENT_LOAD_RESULTS";
			case State::SETUP_DELEGATE_B4_RUNNING: return "SETUP_DELEGATE_B4_RUNNING";
			case State::RUNNING: return "RUNNING";
			default: break;
		}
		ofLogError("ofxApp") << "unknown ofxApp State!";
		return "unknown ofxApp State";
	}

	inline string toString(const Phase& s){
		switch (s) {
			case Phase::WILL_LOAD_CONTENT: return "WILL_LOAD_CONTENT";
			case Phase::DID_DELIVER_CONTENT: return "DID_DELIVER_CONTENT";
			case Phase::WILL_BEGIN_RUNNING: return "WILL_BEGIN_RUNNING";
			default: break;
		}
		ofLogError("ofxApp") << "unknown ofxApp Phase!";
		return "unknown ofxApp Phase";
	}

};


//mostly protocol enforcment for globals & colors
class HasRuiParams{
public:
	virtual void setupRemoteUIParams() = 0;
};
