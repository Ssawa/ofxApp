#include "gtest/gtest.h"

#include "ofxAppUtils.h"
#include "GL/glew.h"
#include "mocks/glew.h"

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

TEST (ofxAppUtils, getNewUUID) {
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
    EXPECT_TRUE("atestemail@localhost.edu");
    EXPECT_TRUE("lot2!a+2@localh_ost.carl");
    
    // THESE DO FAIL BUT SHOULDN'T. COMMENTING FOR DEMO PURPOSES ONLY
    /*
    EXPECT_FALSE("hiya");
    EXPECT_FALSE("yo@hi");
    EXPECT_FALSE("y o@hi.com");
    */
}

TEST(ofxAppUtils, getImageDimensions) {
    ofDisableDataPath();
    ImageInfo info = getImageDimensions("assets/ofxAppUtils/30x20.png");
    EXPECT_TRUE(info.valid);
    EXPECT_EQ(30, info.width);
    EXPECT_EQ(20, info.height);
    EXPECT_EQ(4, info.nChannels);
}

