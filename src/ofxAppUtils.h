//
//  ofxAppUtils.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//


#pragma once
#include "ofMain.h"
#include "ofxAppMacros.h"
#include "ofxThreadSafeLog.h"

namespace ofxApp{
	
	namespace utils{

		struct ImageInfo{
			bool valid;
			int width;
			int height;
			int nChannels;
		};

		void assertFileExists(const string & path);
		
		int haltAndListen(float secondsOnScreen);
		
		void terminateApp(const string & module, const string & reason, float secondsOnScreen = 15);
				
		ofColor createHsbColor(float h, float sat, float brightness); //[0..255.0f]
	
		string secondsToHumanReadable(float secs, int decimalPrecision);
		string bytesToHumanReadable(long long bytes, int decimalPrecision);
		
		string getNewUUID();
		string getFileSystemSafeString(const string & input);

		string getGlInfo();
		string getGlError();

		bool isValidEmail(const string email);

		void logParagraph(const string & moduleName, ofLogLevel lev, const string & text);

		ImageInfo getImageDimensions(const string & filePath);

	} //utils
}; //ofxApp
