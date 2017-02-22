#include "gtest/gtest.h"
#include "mocks/ofBaseRendererMock.h"
#include "ofAppNoWindow.h"
#include "ofMain.h"

using ::testing::_;

class ofExampleApp : public ofBaseApp{
public:
	bool active;
	ofColor color;
	glm::vec2 pos;
	
	void draw() {
		if (this->active) {
			ofSetColor(this->color);
			ofDrawCircle(pos, 5);
		}
	}
};

/*--------------------------------------
shared_ptr<ofBaseRenderer> & ofGetCurrentRenderer(){
	return mainLoop()->getCurrentWindow()->renderer();
}*/

TEST(ofExampleApp, draw) {
	
	// ================ SETUP ================
	class ofAppMockWindow : public ofAppNoWindow {
	public:
		shared_ptr<MockRenderer> mockRenderer = shared_ptr<MockRenderer>(new MockRenderer);
		shared_ptr<ofBaseRenderer> returnRenderer;
		
		ofAppMockWindow() {
			// We need to retain a reference to our derived MockRenderer to perform expectations
			// but we need to return it's base ofBaseRenderer. We can't do this conversion
			// in the renderer() function, however, because the return value would be dependent
			// on temporary variables. Thus, this.
			this->returnRenderer = static_pointer_cast<ofBaseRenderer>(this->mockRenderer);
		}
		
		shared_ptr<ofBaseRenderer> & renderer() {
			return this->returnRenderer;
		}
	};
	
	auto originalWindow = ofGetMainLoop()->getCurrentWindow();
	auto mockWindow = shared_ptr<ofAppMockWindow>(new ofAppMockWindow());
	mockWindow->setup(ofWindowSettings());
	ofGetMainLoop()->setCurrentWindow(mockWindow);
	// ================ END SETUP ================
	
	EXPECT_CALL(*mockWindow->mockRenderer, setColor(0, 255, 0, 255));
	EXPECT_CALL(*mockWindow->mockRenderer, drawCircle(50, 20, 0, 5));
	
	ofExampleApp app;
	
	app.active = true;
	app.color = ofColor(0, 255, 0);
	app.pos = glm::vec2(50, 20);
	app.draw();
	
	app.active = false;
	EXPECT_CALL(*mockWindow->mockRenderer, setColor(_, _, _, _)).Times(0);
	EXPECT_CALL(*mockWindow->mockRenderer, drawCircle(_, _, _, _)).Times(0);
	app.draw();
	
	// ================ TEARDOWN ================
	ofGetMainLoop()->setCurrentWindow(originalWindow);
	mockWindow->returnRenderer.reset();
	mockWindow->mockRenderer.reset();
	mockWindow.reset();
	// ================ END TEARDOWN ================
}
