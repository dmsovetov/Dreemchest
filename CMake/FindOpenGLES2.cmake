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
elseif (ANDROID)
    find_path(OPENGLES2_INCLUDE_DIR GLES2/gl2.h)
    find_library(OPENGLES1_LIBRARY NAMES GLESv1_CM)
    find_library(OPENGLES2_LIBRARY NAMES GLESv2)
    set(OPENGLES2_LIBRARIES ${OPENGLES1_LIBRARY} ${OPENGLES2_LIBRARY})
endif ()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenGLES2 DEFAULT_MSG OPENGLES2_LIBRARY OPENGLES1_LIBRARY OPENGLES2_INCLUDE_DIR)