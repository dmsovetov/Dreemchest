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

//! Indicates that a library was build with libtiff support.
#cmakedefine HAVE_TIFF

//! Indicates that a library was build with OpenGL support.
#cmakedefine DC_OPENGL_ENABLED

//! Indicates that a library was built with Box2D support.
#cmakedefine DC_BOX2D_ENABLED

//! Indicates that a library was built with sound support.
#cmakedefine DC_SOUND_ENABLED

//! A preprocessor constant that indicates a Qt library major version
#cmakedefine DC_QT_VERSION (${DC_QT_VERSION})

#endif  /*  #ifndef __DC_BuildConfig_H__  */
