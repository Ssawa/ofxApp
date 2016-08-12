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

	struct ContentConfig{
		std::function<void (ofxMtJsonParserThread::JsonStructureData &)> describeJsonUserLambda;
		std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseSingleObjectUserLambda;
		std::function<void (ContentObject*)> setupTexturedObjectUserLambda;
	};

	enum State{
		SETTING_UP,
		LOADING_STATIC_TEXTURES,
		LOADING_JSON_CONTENT,
		LOADING_JSON_CONTENT_FAILED,
		LOAD_CUSTOM_USER_CONTENT,
		SETUP_USER_APP,
		POST_USER_SETUP,
		RUNNING,
	};

	inline string toString(State s){
		switch(s){
			case SETTING_UP: return "SETTING_UP";
			case LOADING_STATIC_TEXTURES: return "LOADING_STATIC_TEXTURES";
			case LOADING_JSON_CONTENT: return "LOADING_JSON_CONTENT";
			case LOADING_JSON_CONTENT_FAILED: return "LOADING_JSON_CONTENT_FAILED";
			case LOAD_CUSTOM_USER_CONTENT: return "LOAD_CUSTOM_USER_CONTENT";
			case SETUP_USER_APP: return "SETUP_USER_APP";
			case POST_USER_SETUP: return "POST_USER_SETUP";
			case RUNNING: return "RUNNING";
			default: break;
		}
		ofLogError("ofxApp") << "unknown ofxApp State!";
		return "unknown ofxApp State";
	}

};