# - Try to find OpenGLES and EGL
# Once done this will define
#
#  OPENGLES2_FOUND        - system has OpenGLES
#  OPENGLES2_INCLUDE_DIR  - the GL include directory
#  OPENGLES2_LIBRARIES    - Link these to use OpenGLES
#

if (IOS)
  find_library(OPENGLES2_LIBRARIES OpenGLES DOC "OpenGLES 2 library for iOS")
  find_path(OPENGLES2_INCLUDE_DIR ES2/gl.h DOC "Include for OpenGLES 2")
endif ()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenGLES2 DEFAULT_MSG OPENGLES2_LIBRARIES OPENGLES2_INCLUDE_DIR)