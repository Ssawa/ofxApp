#include "gtest/gtest.h"
#include "ofxAppStaticTextures.h"
#include "mocks/glew.h"

TEST(ofxAppStaticTextures, testing123) {
	glBeginQuery = &mockGlBeginQuery;
	glBeginQuery(GL_TIME_ELAPSED_EXT, 0);
	//ofxAppStaticTextures staticTextures;
	//staticTextures.setup();
}
