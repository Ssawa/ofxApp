#ifndef mock_glew_h
#define mock_glew_h


#include "GL/glew.h"
#include "fff.h"
 
// GoogleMock doesn't support mocking free functions but luckily we have the
// very nifty Fake Function Framework (https://github.com/meekrosoft/fff) which
// gives us somewhat similar behavior. The main thing to keep in mind is that,
// because we don't have the luxury of a creator and destructor you need to
// remember to reset the state of your functions at the end of the test.

DECLARE_FAKE_VALUE_FUNC(GLenum GLAPIENTRY, glGetError);

#endif