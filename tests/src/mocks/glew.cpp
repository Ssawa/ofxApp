#include <mocks/glew.h>
#include "fff.h"

// Mocks out the implementation of GLEW functions so that they don't
// get invoked by the dynamic library.

// Because the compilation and linking of this file can lead to unworking
// behavior for entire applicationslet's be extra safe and make sure we only
// use it when we need it
#ifdef OFXAPP_TESTING

DEFINE_FAKE_VALUE_FUNC(GLenum, glGetError);
DEFINE_FAKE_VOID_FUNC(glGenTextures, GLsizei, GLuint*);


// Decleration for our funky GLEW calls
DEFINE_FAKE_VOID_FUNC(mockGlBeginQuery, GLenum, GLuint);
#endif
