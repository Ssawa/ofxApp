//
//  ofxApp.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesi√† on 3/8/16.
//
//

#include "ofxApp.h"
#include "ofxThreadSafeLog.h"
#include "TexturedObjectStats.h"

ofxApp::App app; //app global in your subclass!

using namespace ofxApp;

void App::setup(ofxApp::UserLambdas cfg, ofxAppDelegate * delegate){

	ofLogNotice("ofxApp") << "setup()";
	if(!this->delegate){

		this->delegate = delegate;
		fontStorage = new AppFonts();
		contentStorage = new AppContent();
		fonts().setup();
		setupLogging();
		loadSettingsBundles();
		setupTextureLoader();
		setupWindow();
		contentCfg = cfg;
		ofxSimpleHttp::createSslContext();
		setupStateMachine();
		appState.setState(SETTING_UP);
		setupListeners();
		setupRemoteUI();
		globals().setupRemoteUIParams();
		setupTimeMeasurements();
		colors().setupRemoteUIParams();
		textures().setup();
		setupTuio();
		setupOF();
		appState.setState(LOADING_STATIC_TEXTURES); //start loading content
	}else{
		ofLogError("ofxApp") << "Trying to setup() ofxApp a second time!";
	}
}


void App::setupOF(){
	ofSetFrameRate(getInt("App/frameRate"));
	ofBackground(colors().bgColor);
	dt = 1.0f / ofGetTargetFrameRate();

	bool showMouse = getBool("App/showMouse");
	if(showMouse) ofShowCursor();
	else ofHideCursor();

	setMouseEvents(getBool("App/showMouse"));
}

void App::setMouseEvents(bool enabled){

	if(enabled){
		if(!ofEvents().mousePressed.isEnabled()){
			ofEvents().mousePressed.enable();
			ofEvents().mouseReleased.enable();
			ofEvents().mouseDragged.enable();
			ofLogWarning("ofxApp") << "Disabled Mouse Events";
		}
	}else{
		if(ofEvents().mousePressed.isEnabled()){
			ofEvents().mousePressed.disable();
			ofEvents().mouseReleased.disable();
			ofEvents().mouseDragged.disable();
			ofLogWarning("ofxApp") << "Enabled Mouse Events";
		}
	}
}


void App::setupWindow(){
	ofxScreenSetup::ScreenMode mode = ofxScreenSetup::ScreenMode((int)getInt("App/window/windowMode"));
	screenSetup.setup(getInt("App/window/customWidth"), getInt("App/window/customHeight"), mode);

	//setup mullions user settings
	bool mullionsVisible = getBool("App/mullions/visibleAtStartup");
	mullions.setup(getInt("App/mullions/numX"), getInt("App/mullions/numY"));
	if(mullionsVisible) mullions.enable();
	else mullions.disable();
}


void App::setupListeners(){

	ofAddListener(ofEvents().update, this, &App::update);
	ofAddListener(ofEvents().exit, this, &App::exit);
	//listen to content manager state changes
	ofAddListener(contentStorage->eventStateChanged, this, &App::onContentManagerStateChanged);
	ofAddListener(ofEvents().keyPressed, this, &App::onKeyPressed);
	ofAddListener(ofEvents().draw, this, &App::draw, OF_EVENT_ORDER_AFTER_APP);
	ofAddListener(textures().eventAllTexturesLoaded, this, &App::onStaticTexturesLoaded);
}

void App::setupStateMachine(){

	//listen to state machine changes
	ofAddListener(appState.eventStateChanged, this, &App::onStateChanged);
	ofAddListener(appState.eventStateError, this, &App::onStateError);
	ofAddListener(appState.eventDraw, this, &App::onDrawLoadingScreenStatus);

	string boldFontPath = getString("Fonts/ofxApp/monospacedBold/fontFile");
	assertFileExists(boldFontPath);
	appState.setup(boldFontPath, "", ofColor::black, ofColor::white);
	//this creates strings for each of the ENUM states
	appState.SET_NAME_AND_COLOR_FOR_STATE(SETTING_UP, ofColor(0,0,255), ofColor(0,0,128));
	appState.SET_NAME_AND_COLOR_FOR_STATE(LOADING_STATIC_TEXTURES, ofColor(0,0,255), ofColor(0,0,128));
	appState.SET_NAME_AND_COLOR_FOR_STATE(LOADING_JSON_CONTENT, ofColor(0,0,255), ofColor(0,0,128));
	appState.SET_NAME_AND_COLOR_FOR_STATE(LOADING_JSON_CONTENT_FAILED, ofColor(255,0,0), ofColor(128,0,0));
	appState.SET_NAME_AND_COLOR_FOR_STATE(LOAD_CUSTOM_USER_CONTENT, ofColor(255,0,0), ofColor(128,0,0));
	appState.SET_NAME_AND_COLOR_FOR_STATE(SETUP_USER_APP, ofColor::white, ofColor::grey);
	appState.SET_NAME_AND_COLOR_FOR_STATE(POST_USER_SETUP, ofColor::white, ofColor::grey);
	appState.SET_NAME_AND_COLOR_FOR_STATE(RUNNING, ofColor::white, ofColor::grey);
}

void App::startLoadingStaticAssets(){
	string texturesPath = getString("StaticAssets/textures");
	assertFileExists(texturesPath);
	textures().loadTexturesInDir(texturesPath, true/*async*/);
}


void App::setupTextureLoader(){

	ProgressiveTextureLoadQueue * q = ProgressiveTextureLoadQueue::instance();
	q->setNumberSimultaneousLoads( getInt("textureLoader/maxNumberSimulataneousLoads") ); //N threads loading images in the bg
	q->setTexLodBias( getFloat("textureLoader/textureLodBias") ); //MipMap sharpness
	q->setTargetTimePerFrame( getFloat("textureLoader/maxTimeSpentLoadingPerFrameMs") );	//spend at most 'x' milis loading textures per frame
	q->setScanlinesPerLoop( getInt("textureLoader/scanlinesPerLoop") );
	q->setMaximumRequestsPerFrame( getInt("textureLoader/maxLoadRequestsPerFrame") );

}

void App::loadSettings(){

	assertFileExists(settingsFile);

	ofLogNotice("ofxApp") << "loadSettings() from " << settingsFile;
	bool ok = settings().load(ofToDataPath(settingsFile, true));
	if(!ok){
		ofLogError("ofxApp") << "Could not load settings from " << ofToDataPath(settingsFile, true);
		terminateApp();
	}
	string settingsString = settings().getAsJsonString();
	ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////";
	ofLogNotice("ofxApp") << "Loaded Settings: " << endl << settingsString << endl;
	ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////";
	hasLoadedSettings = true;
}


void App::saveSettings(){
	ofLogNotice("ofxApp") << "saveSettings() to " << settingsFile;
	settings().save(ofToDataPath(settingsFile, true));
	string settingsString = settings().getAsJsonString();
	ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////";
	ofLogNotice("ofxApp") << "Saved Settings: " << endl << settingsString << endl;
	ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////";
}


void App::setupApp(){

	RUI_NEW_GROUP("APP");
	bool & showMouse = getBool("App/showMouse");
	RUI_SHARE_PARAM(showMouse);
	bool & enableMouse = getBool("App/enableMouse");
	RUI_SHARE_PARAM(enableMouse);
	RUI_PUSH_TO_CLIENT();
	RUI_LOAD_FROM_XML();
	setMouseEvents(enableMouse);
}


void App::setupLogging(){

	#if defined(__has_feature) /*this triggers asan for some reason - dont clean longs when asan is ON*/
		#if !__has_feature(address_sanitizer)
			if(getBool("logging/deleteOldLogs")){
				ofxSuperLog::clearOldLogs(LogsDir, getInt("logging/logExpirationInDays"));
			}
		#endif
	#endif
	bool logToConsole = getBool("logging/toConsole");
	bool logToScreen = getBool("logging/toScreen");
	ofSetLogLevel(ofLogLevel(getInt("logging/logLevel")));
	ofSetLoggerChannel(ofxSuperLog::getLogger(logToConsole, logToScreen, LogsDir));
	ofxSuperLog::getLogger()->setScreenLoggingEnabled(false);
	ofxSuperLog::getLogger()->setMaximized(true);
	ofxSuperLog::getLogger()->setMaxNumLogLines(getInt("logging/maxScreenLines"));
	ofxSuperLog::getLogger()->setUseScreenColors(true);

	if(getBool("logging/useFontStash")){ //set a nice font for the on screen logger if using fontstash
		ofxSuperLog::getLogger()->setFont(&(fonts().getMonoBoldFont()), getFloat("logging/uiScale", 1.0) * getInt("logging/fontSize"));
	}
	float panelW = getFloat("logging/screenLogPanelWidth");
	ofxSuperLog::getLogger()->setDisplayWidth(panelW);

	//asset manager uses this separate logger to create an "asset report"  file after every launch
 	//stating status of every downloaded asset (ie missing sha1, sha1 missmatch, etc)
	ofxThreadSafeLog::one()->setPrintToConsole(getBool("logging/ThreadSafeLog/alsoPrintToConsole"));
}


void App::setupRemoteUI(){
	RUI_SET_CONFIGS_DIR(configsDir);
	RUI_GET_INSTANCE()->setUiColumnWidth(getInt("RemoteUI/columnWidth", 280));
	RUI_GET_INSTANCE()->setBuiltInUiScale(getFloat("RemoteUI/uiScale", 1.0));
	bool useFontStash = getBool("RemoteUI/useFontStash");
	if(useFontStash){
		string fontFile = getString("RemoteUI/fontFile");
		assertFileExists(fontFile);
		RUI_GET_INSTANCE()->drawUiWithFontStash(fontFile, getInt("RemoteUI/fontSize", 15));
	}
	bool ruiSaveOnQuit = getBool("RemoteUI/saveSettingsOnQuit");
	RUI_GET_INSTANCE()->setSaveToXMLOnExit(ruiSaveOnQuit);
	ofLogNotice("ofxApp") << "RemoteUI will save settings on quit: " << ruiSaveOnQuit;
	RUI_GET_INSTANCE()->setShowUIDuringEdits(getBool("RemoteUI/showUiDuringEdits"));

	ofAddListener(RUI_GET_OF_EVENT(), this, &App::onRemoteUINotification);
	RUI_SETUP();
}


void App::loadSettingsBundles(){

	std::pair<string,string> credentials;
	credentials.first = getString("downloads/credentials/username");
	credentials.second = getString("downloads/credentials/password");

	ofxSimpleHttp::ProxyConfig proxyCfg;
	proxyCfg.useProxy = getBool("downloads/proxy/useProxy");
	proxyCfg.host = getString("downloads/proxy/proxyHost");
	proxyCfg.port = getInt("downloads/proxy/proxyPort");
	proxyCfg.login = getString("downloads/proxy/proxyUser");
	proxyCfg.password = getString("downloads/proxy/proxyPassword");

	assetDownloadPolicy.fileMissing = getBool("content/AssetDownloadPolicy/fileMissing");
	assetDownloadPolicy.fileTooSmall = getBool("content/AssetDownloadPolicy/fileTooSmall");
	assetDownloadPolicy.fileExistsAndNoSha1Provided = getBool("content/AssetDownloadPolicy/fileExistsAndNoSha1Provided");
	assetDownloadPolicy.fileExistsAndProvidedSha1Missmatch = getBool("content/AssetDownloadPolicy/fileExistsAndProvidedSha1Missmatch");
	assetDownloadPolicy.fileExistsAndProvidedSha1Match = getBool("content/AssetDownloadPolicy/fileExistsAndProvidedSha1Match");

	assetUsagePolicy.fileMissing = getBool("content/AssetUsagePolicy/fileMissing");
	assetUsagePolicy.fileTooSmall = getBool("content/AssetUsagePolicy/fileTooSmall");
	assetUsagePolicy.fileExistsAndNoSha1Provided = getBool("content/AssetUsagePolicy/fileExistsAndNoSha1Provided");
	assetUsagePolicy.fileExistsAndProvidedSha1Missmatch = getBool("content/AssetUsagePolicy/fileExistsAndProvidedSha1Missmatch");
	assetUsagePolicy.fileExistsAndProvidedSha1Match = getBool("content/AssetUsagePolicy/fileExistsAndProvidedSha1Match");

	objectUsagePolicy.allObjectAssetsAreOK = getBool("content/ObjectUsagePolicy/allAssetsAreOK");
	objectUsagePolicy.minNumberOfImageAssets = getBool("content/ObjectUsagePolicy/minNumberImgAssets");
	objectUsagePolicy.minNumberOfVideoAssets = getBool("content/ObjectUsagePolicy/minNumberVideoAssets");
	objectUsagePolicy.minNumberOfAudioAssets = getBool("content/ObjectUsagePolicy/minNumberAudioAssets");

	renderSize.x = getInt("App/renderSize/width");
	renderSize.y = getInt("App/renderSize/height");
}

void App::setupRuiWatches(){

	ofxJSON paramWatches = settings().getJson("RemoteUI/paramWatches");
	if(paramWatches.size()){
		for( auto itr = paramWatches.begin(); itr!=paramWatches.end() ; itr++){
			string paramName = itr.key().asString();
			bool shouldWatch = (*itr).asBool();
			if (shouldWatch){
				ofLogNotice("ofxApp") << "Adding RemoteUI Param Watch for '" << paramName << "'";
				RUI_WATCH_PARAM_WCN(paramName);
			}
		}
	}
}


void App::setupTimeMeasurements(){
	TIME_SAMPLE_SET_CONFIG_DIR(configsDir);
	TIME_SAMPLE_SET_FRAMERATE(getInt("App/frameRate", 60));
	TIME_SAMPLE_SET_ENABLED(getBool("TimeMeasurements/enabled", true));
	TIME_SAMPLE_DISABLE_AVERAGE();
	TIME_SAMPLE_SET_DRAW_LOCATION((ofxTMDrawLocation)(getInt("TimeMeasurements/widgetLocation", 3)));
	TIME_SAMPLE_GET_INSTANCE()->setDeadThreadTimeDecay(getFloat("TimeMeasurements/threadTimeDecay"));
	TIME_SAMPLE_GET_INSTANCE()->setUiScale(getFloat("TimeMeasurements/uiScale", 1.0));
	bool useFontStash = getBool("TimeMeasurements/useFontStash");
	if(useFontStash){
		string fontFile = getString("TimeMeasurements/fontFile");
		assertFileExists(fontFile);
		TIME_SAMPLE_GET_INSTANCE()->drawUiWithFontStash(fontFile, getInt("TimeMeasurements/fontSize", 13));
	}
	TIME_SAMPLE_GET_INSTANCE()->setMsPrecision(getInt("TimeMeasurements/msPrecision", 2));
	TIME_SAMPLE_GET_INSTANCE()->setPlotResolution(getFloat("TimeMeasurements/plotResolution", 1.0));
	TIME_SAMPLE_SET_REMOVE_EXPIRED_THREADS(getBool("TimeMeasurements/removeExpiredThreads", true));
	TIME_SAMPLE_GET_INSTANCE()->setRemoveExpiredTimings(getBool("TimeMeasurements/removeExpiredTimings", false));
	TIME_SAMPLE_GET_INSTANCE()->setDrawPercentageAsGraph(getBool("TimeMeasurements/percentageAsGraph", true));
	TIME_SAMPLE_GET_INSTANCE()->setPlotHeight(getFloat("TimeMeasurements/plotH", 60));
}


void App::setupTuio(){
	if(getBool("tuio/enabled")){
		int port = getInt("tuio/port");
		ofLogNotice("ofxApp") << "Listening for TUIO events at port " << port;
		tuioClient.start(port); //TODO - make sure we do it only once!
		ofAddListener(tuioClient.cursorAdded, delegate, &ofxAppDelegate::tuioAdded);
		ofAddListener(tuioClient.cursorRemoved, delegate, &ofxAppDelegate::tuioRemoved);
		ofAddListener(tuioClient.cursorUpdated, delegate, &ofxAppDelegate::tuioUpdated);
	}
}



void App::update(ofEventArgs &){
	tuioClient.getMessage();
	contentStorage->update(dt);
	updateStateMachine(dt);
}

void App::exit(ofEventArgs &){
	ofLogWarning("ofxApp") << "OF is exitting!";
	ofLogWarning("ofxApp") << "destroying ssl context...";
	ofxSimpleHttp::destroySslContext();
	ofLogWarning("ofxApp") << "closing logs...";
	ofxThreadSafeLog::one()->close();
	ofLogWarning("ofxApp") << "done exitting!";
}

void App::onStaticTexturesLoaded(){
	ofLogNotice("ofxApp")<< "All Static Textures Loaded!";
	appState.setState(LOADING_JSON_CONTENT);
}


//////////////////// LOADING SCREEN /////////////////////////////////////////////////////////////////
#pragma Draw Loading Screen

void App::draw(ofEventArgs &){
	if(appState.getState() != RUNNING){
		ofSetupScreen();
		appState.draw(ofGetCurrentViewport());
	}
	ofSetColor(0);
	mullions.draw();
	ofSetColor(255);

	//stack up stats
	int x = 20;
	int y = 30;
	int lineH = 14;

	if(globalsStorage.drawStaticTexturesMemStats){
		float mb = app.textures().getTotalMemUsed();
		ofDrawBitmapStringHighlight("Static Assets Mem Used: " + ofToString(mb, 1) + "Mb", ofVec2f(x,y), ofColor::black, ofColor::fuchsia);
		y += lineH * 2;
	}

	if(globalsStorage.drawTextureLoaderStats){
		TexturedObjectStats::one().draw(x, y);
		y += lineH * 8;
	}

	if(globalsStorage.drawTextureLoaderState){
		ProgressiveTextureLoadQueue::instance()->draw(x, y);
		y += lineH * 4 + ProgressiveTextureLoadQueue::instance()->getNumBusy() * lineH;
	}

}


void App::onDrawLoadingScreenStatus(ofRectangle & area){

	switch (appState.getState()) {

		case LOADING_STATIC_TEXTURES:{
			ofTexture * tex = textures().getLatestLoadedTex();
			float progress = textures().getNumLoadedTextures() / float(textures().getNumTextures());
			appState.updateState( progress, "");
			if(tex){
				ofRectangle tr = ofRectangle(0,0,tex->getWidth(), tex->getHeight());
				tr.scaleTo(area, OF_SCALEMODE_FIT);
				tex->draw(tr);
				ofPushStyle();
				ofSetColor(0);
				fonts().getMonoBoldFont().draw(ofToString(textures().getTotalMemUsed(), 1) + "MBytes used", loadingScreenFontSize, 101, 101);
				ofSetColor(255);
				fonts().getMonoBoldFont().draw(ofToString(textures().getTotalMemUsed(), 1) + "MBytes used", loadingScreenFontSize, 100, 100);
				ofPopStyle();
			}
		}break;

		case LOAD_CUSTOM_USER_CONTENT:
		case SETUP_USER_APP:
			delegate->drawLoadingScreenForUserProcess(appState.getState(), area);
			break;

		default: break;
	}
}


#pragma mark State Machine

void App::updateStateMachine(float dt){

	switch (appState.getState()) {

		case LOADING_JSON_CONTENT:
			appState.updateState( contentStorage->getPercentDone(), contentStorage->getStatus());

			if(appState.isReadyToProceed() ){ //slow down the state machine to handle error / retry

				if( appState.hasError() && appState.ranOutOfErrorRetries()){ //give up!
					ofLogError("ofxApp") << "json failed to load too many times! Giving Up!";
					appState.setState(LOADING_JSON_CONTENT_FAILED);
					break;
				}else{
					if(contentStorage->isContentReady()){ //see if we are done (optional)
						ofLogNotice("ofxApp") << "JSON content loaded!";
						appState.setState(LOAD_CUSTOM_USER_CONTENT);
						break;
					}
				}

				if(contentStorage->foundError()){

					int numRetries = getInt("StateMachine/onErrorRetryCount", 5);
					int delaySeconds = getInt("StateMachine/onErrorWaitTimeSec", 5);
					appState.setError("failed to load!", delaySeconds /*sec*/, numRetries /*retry max*/); //report an error, retry!
					ofLogError("ofxApp") << "json failed to load! (" << appState.getNumTimesRetried() << ")";
					if(numRetries > 0){ //if no retry allowed, jump to fail state directly
						appState.setState(LOADING_JSON_CONTENT, false); //note "false" << do not clear errors (to keep track of # of retries)
					}else{
						appState.setState(LOADING_JSON_CONTENT_FAILED);
					}
				}
			}
			break;

		case LOADING_JSON_CONTENT_FAILED:
			appState.updateState( -1, "error while loading content!");
			if (appState.getElapsedTimeInCurrentState() > 15){ //hold the error screen for a while and quit
				ofLogFatalError("ofxApp") << "cant load json, exiting!";
				terminateApp();
			}
			break;

		case RUNNING:
			appState.updateState( -1, "");
			break;

		case LOAD_CUSTOM_USER_CONTENT:
			if(delegate->isUserProcessDone(LOAD_CUSTOM_USER_CONTENT)){
				ofLogNotice("ofxApp") << "done loading Custom User Content!";
				appState.setState(SETUP_USER_APP);
			}
			break;

		case SETUP_USER_APP:
			if(delegate->isUserProcessDone(SETUP_USER_APP)){
				ofLogNotice("ofxApp") << "done Setup User App!";
				appState.setState(POST_USER_SETUP);
			}
			break;

		default: break;
	}
}


void App::onStateChanged(ofxStateMachine<ofxApp::State>::StateChangedEventArgs& change){

	ofLogNotice("ofxApp") 	<< "State Changed from " << appState.getNameForState(change.oldState)
							<< " to " << appState.getNameForState(change.newState);

	switch(change.newState){

		case LOADING_STATIC_TEXTURES:
			startLoadingStaticAssets();
			break;

		case LOADING_JSON_CONTENT:{

			string jsonURL = getString("content/urls/jsonContentURL");
			string jsonDir = getString("content/jsonDownloadDir");
			int numConcurrentDownloads = getInt("downloads/maxConcurrentDownloads");
			int numThreads = getInt("App/maxThreads");
			int timeOutSecs = getInt("downloads/timeOutSec");
			int speedLimitKBs = getInt("downloads/speedLimitKb");
			float idleTimeAfterDl = getFloat("downloads/idleTimeAfterEachDownloadSec");

			contentStorage->setup(	jsonURL,
								 	jsonDir,
									numThreads,
									numConcurrentDownloads,
								 	speedLimitKBs,
								 	timeOutSecs,
								 	idleTimeAfterDl,
								 	credentials,
								 	proxyCfg,
									contentCfg,
									objectUsagePolicy
								 );
			contentStorage->fetchContent(); //this starts the AppContent process! TODO multiple sources!
			}break;

		case LOADING_JSON_CONTENT_FAILED:
			appState.setProgressBarExtraInfo(" - CONTENT LOAD FAILED");
			ofxSuperLog::getLogger()->setScreenLoggingEnabled(true); //show log if json error
			break;

		case LOAD_CUSTOM_USER_CONTENT:
			delegate->contentIsReady(content().getParsedObjects());
			ofLogNotice("ofxApp") << "start loading Custom User Content...";
			delegate->startUserProcess(LOAD_CUSTOM_USER_CONTENT);
			break;

		case SETUP_USER_APP:
			ofLogNotice("ofxApp") << "start loading Custom User Content...";
			delegate->startUserProcess(SETUP_USER_APP);
			break;

		case POST_USER_SETUP:
			setupRuiWatches();
			setupApp();
			appState.setState(RUNNING);
			break;

		case RUNNING: break;

		default: break;
	}
}


void App::onStateError(ofxStateMachine<ofxApp::State>::ErrorStateEventArgs& error){
	ofLogError("ofxApp") << "Error '" << error.errorMsg << "' during state '" << appState.getNameForState(error.state) << "'";
}


#pragma mark !!
void App::onContentManagerStateChanged(string& s){
	appState.setProgressBarExtraInfo(": " + s); // add our sub-state name to the loading screen
}

ofRectangle App::getRenderAreaForCurrentWindowSize(){

	ofRectangle win = ofRectangle(0,0, ofGetWindowWidth(), ofGetWindowHeight());
	ofRectangle render = ofRectangle(0,0,app.renderSize.x, app.renderSize.y);
	render.scaleTo(win);
	return render;
}

void App::onRemoteUINotification(RemoteUIServerCallBackArg &arg){
	switch (arg.action) {
		case CLIENT_UPDATED_PARAM:
			if(arg.paramName == "showMouse"){
				if(arg.param.boolVal) ofShowCursor();
				else ofHideCursor();
			}
			if(arg.paramName == "enableMouse"){
				setMouseEvents(arg.param.boolVal);
			}
			if(arg.paramName == "bgColor"){
				//colorsStorage.bgColor = arg.param.getColor();
				ofBackground(colorsStorage.bgColor);
				RUI_PUSH_TO_CLIENT();
			}

			break;
		default:
			break;
	}
}


void App::onKeyPressed(ofKeyEventArgs & a){
	bool didPress = false;
	switch(a.key){
		case 'W': screenSetup.cycleToNextScreenMode(); didPress = true; break;
		case 'L': ofxSuperLog::getLogger()->setScreenLoggingEnabled(!ofxSuperLog::getLogger()->isScreenLoggingEnabled()); didPress = true; break;
		case 'M': mullions.toggle(); didPress = true; break;
		case 'D': globalsStorage.debug^= true; didPress = true; break;
	}
	if(didPress){
		RUI_PUSH_TO_CLIENT();
	}
}



///////////////////// SETTINGS //////////////////////////////////////////////////////////////////////
#pragma mark Settings

bool& App::getBool(const string & key, bool defaultVal){
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(settings().exists(key)){
		if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << FILE_ACCES_ICON << " getting Bool Value for '" << key << "' : " << settings().getBool(key);
		return settings().getBool(key);
	}else{
		ofLogFatalError("ofxApp") << "Requesting setting that does not exist! " << key;
		if(QUIT_ON_MISSING_SETTING) terminateApp();
		static auto def = defaultVal;
		return def; //mmmm....
	}
}


int& App::getInt(const string & key, int defaultVal){
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(settings().exists(key)){
		if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << FILE_ACCES_ICON << " getting Int Value for '" << key << "' : " << settings().getInt(key);
		return settings().getInt(key);
	}else{
		ofLogFatalError("ofxApp") << "Requesting setting that does not exist! " << key;
		if(QUIT_ON_MISSING_SETTING) terminateApp();
		static auto def = defaultVal;
		return def; //mmmm....
	}
}

float& App::getFloat(const string & key, float defaultVal){
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(settings().exists(key)){
		if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << FILE_ACCES_ICON << " getting Float Value for '" << key << "' : " << settings().getFloat(key);
		return settings().getFloat(key);
	}else{
		ofLogFatalError("ofxApp") << "Requesting setting that does not exist! " << key;
		if(QUIT_ON_MISSING_SETTING) terminateApp();
		static auto def = defaultVal;
		return def; //mmmm....
	}
}

string& App::getString(const string & key, string defaultVal){
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(settings().exists(key)){
		if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << " getting String Value for '" << key << "' : " << settings().getString(key);
		return settings().getString(key);
	}else{
		ofLogFatalError("ofxApp") << "Requesting setting that does not exist! " << key;
		if(QUIT_ON_MISSING_SETTING) terminateApp();
		static auto def = defaultVal;
		return def; //mmmm....
	}
}

ofColor& App::getColor(const string & key, ofColor defaultVal){
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(settings().exists(key)){
		if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << FILE_ACCES_ICON << " getting Color Value for '" << key << "' : " << settings().getColor(key);
		return settings().getColor(key);
	}else{
		ofLogFatalError("ofxApp") << "Requesting setting that does not exist! " << key;
		if(QUIT_ON_MISSING_SETTING) terminateApp();
		static auto def = defaultVal;
		return def; //mmmm....
	}
}
