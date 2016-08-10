//
//  ofxApp.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 3/8/16.
//
//

#pragma once

#define VERBOSE_SETTINGS_ACCESS 	true
#define QUIT_ON_MISSING_SETTING		true

//Check if the user created the required macro to include his custom sub-classes for Colors, Globals and Fonts.
#ifndef OFX_APP_NAME
	#error You Must define an app Name for your app in the preprocessor macros; ie OFX_APP_NAME=MyApp
	//#define OFX_APP_NAME MyApp /*you define your App's name in your PREPROCESSOR MACROS*/
#endif

#include "ofxAppMacros.h"
#include "ofMain.h"
#include "ofxJsonSettings.h"

//some macro magic to include the user defined subclasses of AppColorsBasic, AppFontsBasic, AppGlobalsBasic
//it takes the user defined macro (ie OFX_APP_NAME=myApp) and creates an #include "myAppColors.h"
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_COLORS_FILENAME) 	//include myAppColors.h
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_FONTS_FILENAME)	//include myAppFonts.h
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_GLOBALS_FILENAME) //include myAppGlobals.h
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_STATES_FILENAME)	//include myAppStates.h
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_CONTENT_FILENAME)	//include myAppContent.h

#include "ofxTuio.h"
#include "ofxSuperLog.h"
#include "ofxScreenSetup.h"
#include "ofxRemoteUIServer.h"
#include "ofxTimeMeasurements.h"
#include "AppBaseClasses.h"
#include "AppStaticTextures.h"
#include "ofxDrawableStateMachine.h"

class ofxApp : public HasAssets{

public:

	const string settingsFile = "configs/AppSettings.json";
	const string LogsDir = "logs";
	const string configsDir = "configs";

	static ofxApp& one(){
		static ofxApp instance; // Instantiated on first use.
		return instance;
	}

	ofxApp();

	// APP STATES //////////////////////////////////////////////////////////////////////////////////
	//here we create an enum by including differnt files -
	// 1st #include is the most essential states defined as part of the ofxApp
	// 2nd #include is for you to define your own custom states - whatever you need for your app
	// for this to work, you must do as stated above: create pre-Proc Macro (OFX_APP_NAME=myApp) naming your app
 	//and create a file named "myAppStates.h" which contains your extra states
	enum AppState{
		#include "AppStatesBasic.h"
		#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_STATES_FILENAME)
	};

	void setup();
	void loadStaticAssets();
	void postSetup();

	void loadSettings();
	void saveSettings();

	void update(float dt);


	ofxJsonSettings& settings(){return ofxJsonSettings::get();}

	// Crazy Macro magic here!! Beware!!
	// this compounds some classnames to match whatever you decided to name your app;
	// so "OFX_APP_CLASS_NAME(Colors)" becomes "MyAppColors"
	// "MyApp" is a macro you MUST define in your pre-processor macros:
	//  OFX_APP_NAME=MyApp
	OFX_APP_CLASS_NAME(Colors) & colors(){return colorsStorage;}
	OFX_APP_CLASS_NAME(Globals) & globals(){return globalsStorage;}
	OFX_APP_CLASS_NAME(Fonts) & fonts(){return fontStorage;}

	AppStaticTextures & textures(){return texStorage;}

	ofPtr<ofxSuperLog> logger(){return ofxSuperLog::getLogger();}
	ofxTuioClient & tuio(){ return tuioClient;}

	ofxScreenSetup screenSetup;

	// Convinience Getters for App Settings ////////////////////////////////////////////////////////
	inline bool& getBool(const string & key, bool defaultVal = true);
	inline int& getInt(const string & key, int defaultVal = 0);
	inline float& getFloat(const string & key, float defaultVal = 0.0);
	inline string& getString(const string & key, string defaultVal = "uninited!");

	// TUIO ////////////////////////////////////////////////////////////////////////////////////////
	virtual void tuioAdded(ofxTuioCursor & tuioCursor){};
	virtual void tuioRemoved(ofxTuioCursor & tuioCursor){};
	virtual void tuioUpdated(ofxTuioCursor & tuioCursor){};
	ofxTuioCursor getTuioAtMouse(int x, int y);




	// STATE MACHINE ///////////////////////////////////////////////////////////////////////////////
	ofxDrawableStateMachine<AppState> appState; //App State Machine
	virtual void updateStateMachine(float dt);


	// CALLBACKS ///////////////////////////////////////////////////////////////////////////////////
	void remoteUIClientDidSomething(RemoteUIServerCallBackArg & arg);

protected:

	void setupTimeMeasurements();
	void setupRemoteUI();
	void setupLogging();
	void setupTuio(int port = 3333);
	void setupApp();
	void setupRuiWatches();

	ofxTuioClient						tuioClient;
	AppStaticTextures					texStorage;

	//crazy macro magic - beware! read a few lines above to see what's going on
	OFX_APP_CLASS_NAME(Colors)			colorsStorage;
	OFX_APP_CLASS_NAME(Globals)			globalsStorage;
	OFX_APP_CLASS_NAME(Fonts)			fontStorage;
	OFX_APP_CLASS_NAME(Content)			contentStorage;

	bool 								isSetup = false;
	bool								hasLoadedSettings = false;
	//bool								shouldQuit;
};


//extern ofxApp app; //all global parameters are here - add yor "ofxApp" subclass!