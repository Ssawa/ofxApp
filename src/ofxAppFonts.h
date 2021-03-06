//
//  ofxAppFonts.h
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#pragma once
#include "ofMain.h"
#include "ofxFontStash.h"

class ofxAppFonts{

public:

	void setup();

	ofxFontStash& getMonoFont(){return monospaced;}
	ofxFontStash& getMonoBoldFont(){return monospacedBold;}
	ofxFontStash* getFont(const string& name);

protected:

	void loadUseFonts();
	void loadOfxAppFont(const string & key, ofxFontStash & font);

	ofxFontStash monospaced;
	ofxFontStash monospacedBold;

	std::map<string, ofxFontStash*> userFonts;
};