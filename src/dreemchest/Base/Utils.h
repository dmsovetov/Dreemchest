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

#ifndef __Base_H__
#define __Base_H__

#define FOO_INCLUDED

#ifdef FOO_NAMESPACE
    #define DC_BEGIN_DREEMCHEST namespace FOO_NAMESPACE {
    #define DC_END_DREEMCHEST   }
    #define USING_FOO using namespace FOO_NAMESPACE;
#else
    #define DC_BEGIN_DREEMCHEST
    #define DC_END_DREEMCHEST
    #define USING_FOO
#endif

#include "Preprocessor.h"
#include "Exception.h"
#include "Logger.h"
#include "StringHash.h"
#include "Format.h"
#include "Classes.h"
#include "Bitset.h"
#include "UserData.h"
#include "Guid.h"
#include "Composition.h"

DC_BEGIN_DREEMCHEST
    #include "delegate/Closure.h"
DC_END_DREEMCHEST

#include "memory/WeakPtr.h"
#include "memory/StrongPtr.h"
#include "memory/AutoPtr.h"

#include "Types.h"
#include "Image.h"

#include "Variant.h"

#endif  /*  !defined( __Base_H__ ) */
