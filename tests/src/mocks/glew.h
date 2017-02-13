#ifndef mock_glew_h
#define mock_glew_h

// Mocks out the implementation of GLEW functions so that they don't
// get invoked by the dynamic library.

// Because the compilation and linking of this file can lead to unworking
// behavior for entire applicationslet's be extra safe and make sure we only
// use it when we need it
#ifdef OFXAPP_TESTING

#include "GL/glew.h"
#include "fff.h"
 
// GoogleMock doesn't support mocking free functions but luckily we have the
// very nifty Fake Function Framework (https://github.com/meekrosoft/fff) which
// gives us somewhat similar behavior. The main thing to keep in mind is that,
// because we don't have the luxury of a creator and destructor you need to
// remember to reset the state of your functions at the end of the test.
DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(GLenum GLAPIENTRY, glGetError);

#endif

#endif
