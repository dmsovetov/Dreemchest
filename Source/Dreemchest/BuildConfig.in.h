/**************************************************************************
 
 The MIT License (MIT)
 
 Copyright (c) 2015 Dmitry Sovetov
 
 https://github.com/dmsovetov
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 **************************************************************************/

#ifndef __DC_BuildConfig_H__
#define __DC_BuildConfig_H__

//! An md5 checksum of a build configuration to make sure that header files and a built library match.
#cmakedefine DREEMCHEST_VERSION "${DREEMCHEST_VERSION}"

//! A library namespace.
#cmakedefine DC_NAMESPACE ${DC_NAMESPACE}

//! A target platform identifier string.
#cmakedefine DC_PLATFORM "${DC_PLATFORM}"

//! Indicates that a library was build with OpenGL support.
#cmakedefine DC_OPENGL_ENABLED

//! Indicates that a library was built with sound support.
#cmakedefine DC_SOUND_ENABLED

//! A preprocessor constant that indicates a Qt library major version.
#cmakedefine DC_QT_VERSION (${DC_QT_VERSION})

//! Indicates that a jsoncpp library was found upon configuration process.
#cmakedefine JSONCPP_FOUND

//! Indicates that a zlib library was found upon configuration process.
#cmakedefine ZLIB_FOUND

//! Indicates that a Box2D library was found upon configuration process.
#cmakedefine BOX2D_FOUND

//! Indicates that a OpenAL library was found upon configuration process.
#cmakedefine OPENAL_FOUND

//! Indicates that a Ogg library was found upon configuration process.
#cmakedefine OGG_FOUND

//! Indicates that a Vorbis library was found upon configuration process.
#cmakedefine VORBIS_FOUND

//! Indicates that a Google Test library was found upon configuration process.
#cmakedefine GTEST_FOUND

//! Indicates that a libpng library was found upon configuration process.
#cmakedefine PNG_FOUND

//! Indicates that a libtiff library was found upon configuration process.
#cmakedefine TIFF_FOUND

//! Indicates that a Lua library was found upon configuration process.
#cmakedefine LUA_FOUND

//! Indicates that a cURL library was found upon configuration process.
#cmakedefine CURL_FOUND

//! Define a combined macro definition that indicates that both Ogg and Vorbis libraries were found
#if defined(OGG_FOUND) && defined(VORBIS_FOUND)
    #define OGGVORBIS_FOUND
#endif  /*  defined(OGG_FOUND) && defined(VORBIS_FOUND) */

#endif  /*  #ifndef __DC_BuildConfig_H__  */
