#ifndef mock_glew_h
#define mock_glew_h


#include "GL/glew.h"
#include "fff.h"
 
// GoogleMock doesn't support mocking free functions but luckily we have the
// very nifty Fake Function Framework (https://github.com/meekrosoft/fff) which
// gives us somewhat similar behavior. The main thing to keep in mind is that,
// because we don't have the luxury of a creator and destructor you need to
// remember to reset the state of your functions at the end of the test.

DECLARE_FAKE_VALUE_FUNC(GLenum, glGetError);
DECLARE_FAKE_VOID_FUNC(glGenTextures, GLsizei, GLuint*);

// This is all just for reference:
//
// Unlike functions such as "glGenTextues" which are simply function declerations,
// something like "glBeginQuery" is actually a function *pointer* that is initialized
// by GLEW. How GLEW decided which should be which I do not know. The point is, we
// can't just define/declare these functions directly using fff otherwise we'll be
// trying to redefine the function as a totally different symbol (as a function rather
// than a function pointer). Instead we need to define our mock function and then point
// these functions to their memory location.
extern "C" void mockGlBeginQuery(GLenum, GLuint);
DECLARE_FAKE_VOID_FUNC(mockGlBeginQuery, GLenum, GLuint);
//
// You would than call
//     glBeginQuery = &mockGlBeginQuery
// at the beginning of your test and be all set. This can all obviously be put into
// a macro, but I don't have the patience for that.
#endif
