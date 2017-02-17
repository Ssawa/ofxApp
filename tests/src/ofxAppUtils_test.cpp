#include "gtest/gtest.h"

#include "ofxAppUtils.h"
#include "GL/glew.h"
#include "mocks/glew.h"
#include "mocks/ofAppWindowMock.h"

using namespace ofxApp::utils;

vector<string> splitString(std::string value, char delim) {
    std::stringstream ss(value);
    vector<string> tokens;
    string item;
    while (std::getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}



TEST(ofxAppUtils, haltAndListen) {
	auto start = std::chrono::system_clock::now();
	float sleepAmount = .2;
	haltAndListen(sleepAmount);
	auto stop = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = stop - start;
	EXPECT_GE(diff.count(), sleepAmount);
	EXPECT_LT(diff.count(), sleepAmount + .2); // This is kind of fuzzy. Our calculation obviously won't be exactly .5 but how much leeway can we give it while also assuring that we're not seeing a logical error?
}

TEST(ofxAppUtils, haltAndListenEarlyExit) {
	auto originalWindow = ofGetMainLoop()->getCurrentWindow();
	auto dummyWindow = shared_ptr<ofAppNoWindowAlwaysClose>(new ofAppNoWindowAlwaysClose());
	dummyWindow->setup(ofWindowSettings());
	ofGetMainLoop()->setCurrentWindow(dummyWindow);
	
	auto start = std::chrono::system_clock::now();
	float sleepAmount = 100;
	haltAndListen(sleepAmount);
	auto stop = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = stop - start;
	EXPECT_LT(diff.count(), .2);
	
	ofGetMainLoop()->setCurrentWindow(originalWindow);
	dummyWindow.reset();
}

TEST(ofxAppUtils, secondsToHumanReadable) {
    EXPECT_EQ(secondsToHumanReadable(50, 3), "50.000 seconds");
    
    EXPECT_EQ(secondsToHumanReadable(155, 3), "2.583 minutes");
    EXPECT_EQ(secondsToHumanReadable(155, 5), "2.58333 minutes");
    
    EXPECT_EQ(secondsToHumanReadable(8332, 3), "2.314 hours");
    EXPECT_EQ(secondsToHumanReadable(8332, 5), "2.31444 hours");
    
    EXPECT_EQ(secondsToHumanReadable(234125, 3), "2.710 days");
    EXPECT_EQ(secondsToHumanReadable(234125, 5), "2.70978 days");
    
    EXPECT_EQ(secondsToHumanReadable(1233452, 3), "2.039 weeks");
    EXPECT_EQ(secondsToHumanReadable(1233452, 5), "2.03944 weeks");
}

TEST(ofxAppUtils, bytesToHumanReadable) {
    EXPECT_EQ(bytesToHumanReadable(50, 3), "50 bytes");
    
    EXPECT_EQ(bytesToHumanReadable(1025, 3), "1.001 KB");
    EXPECT_EQ(bytesToHumanReadable(1025, 5), "1.00098 KB");
    
    EXPECT_EQ(bytesToHumanReadable(1048979, 3), "1.000 MB");
    EXPECT_EQ(bytesToHumanReadable(1048979, 5), "1.00038 MB");

    EXPECT_EQ(bytesToHumanReadable(2073751823, 3), "1.931 GB");
    EXPECT_EQ(bytesToHumanReadable(2073751823, 5), "1.93133 GB");

}

TEST(ofxAppUtils, getNewUUID) {
    vector<string> uuidSegments = splitString(getNewUUID(), '-');
    
    ASSERT_EQ(uuidSegments.size(), 5);
    
    EXPECT_EQ(uuidSegments[0].length(), 8);
    
    EXPECT_EQ(uuidSegments[1].length(), 4);
    
    EXPECT_EQ(uuidSegments[2][0], '4');
    EXPECT_EQ(uuidSegments[2].length(), 4);
    
    EXPECT_EQ(uuidSegments[3][0], 'a');
    EXPECT_EQ(uuidSegments[3].length(), 4);
    
    EXPECT_EQ(uuidSegments[4].length(), 12);
    
}

TEST(ofxAppUtils, getFileSystemSafeString) {
    EXPECT_EQ(getFileSystemSafeString("t\\gf?d>>>>h"), "t_gf_d____h");
}

TEST(ofxAppUtils, getGLError) {
    glGetError_fake.return_val = GL_INVALID_ENUM;
    EXPECT_EQ(getGlError(), "GL_INVALID_ENUM");
    EXPECT_EQ(glGetError_fake.call_count, 1);

    glGetError_fake.return_val = GL_INVALID_VALUE;
    EXPECT_EQ(getGlError(), "GL_INVALID_VALUE");
    EXPECT_EQ(glGetError_fake.call_count, 2);

    glGetError_fake.return_val = GL_INVALID_OPERATION;
    EXPECT_EQ(getGlError(), "GL_INVALID_OPERATION");
    EXPECT_EQ(glGetError_fake.call_count, 3);
    
    glGetError_fake.return_val = GL_INVALID_FRAMEBUFFER_OPERATION;
    EXPECT_EQ(getGlError(), "GL_INVALID_FRAMEBUFFER_OPERATION");
    EXPECT_EQ(glGetError_fake.call_count, 4);
    
    glGetError_fake.return_val = GL_OUT_OF_MEMORY;
    EXPECT_EQ(getGlError(), "GL_OUT_OF_MEMORY");
    EXPECT_EQ(glGetError_fake.call_count, 5);
    
    glGetError_fake.return_val = GL_STACK_UNDERFLOW;
    EXPECT_EQ(getGlError(), "GL_STACK_UNDERFLOW");
    EXPECT_EQ(glGetError_fake.call_count, 6);
    
    glGetError_fake.return_val = GL_STACK_OVERFLOW;
    EXPECT_EQ(getGlError(), "GL_STACK_OVERFLOW");
    EXPECT_EQ(glGetError_fake.call_count, 7);
    
    RESET_FAKE(glGetError);
}

TEST(ofxAppUtils, isValidEmail) {
    EXPECT_TRUE(isValidEmail("atestemail@localhost.edu"));
    EXPECT_TRUE(isValidEmail("lot2!a+2@localh_ost.carl"));
    
    EXPECT_FALSE(isValidEmail("hiya"));
    EXPECT_FALSE(isValidEmail("yo@hi"));
    EXPECT_FALSE(isValidEmail("y o@hi.com"));
}

TEST(ofxAppUtils, getImageDimensions) {
    ofDisableDataPath();
    ImageInfo info = getImageDimensions("assets/ofxAppUtils/30x20.png");
    EXPECT_TRUE(info.valid);
    EXPECT_EQ(30, info.width);
    EXPECT_EQ(20, info.height);
    EXPECT_EQ(4, info.nChannels);
}

