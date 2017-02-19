#include "gtest/gtest.h"
#include "ofxAppStaticTextures.h"
#include "mocks/glew.h"

TEST(ofxAppStaticTextures, testing123) {
	ofxAppStaticTextures staticTextures;
	
	EXPECT_TRUE(staticTextures.setup());
	
	EXPECT_FALSE(staticTextures.setup());
}
