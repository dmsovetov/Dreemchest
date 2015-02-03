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

#ifndef        __DC_TYPES_H__
#define        __DC_TYPES_H__

#include    <string>
#include    <vector>
#include    <list>
#include    <stdarg.h>
#include    <map>
#include    <queue>

namespace dreemchest {

    typedef double                  f64;
    typedef float                   f32;
    typedef char                    s8;
    typedef signed long long        s64;
    typedef signed int              s32;
    typedef unsigned long long      u64;
    typedef unsigned int            u32;
    typedef unsigned short          u16;
    typedef unsigned char           u8;
    typedef StringHash::strhash     strhash;
    typedef std::string             String;
    typedef std::vector<String>     StringArray;

} // namespace dreemchest

#endif        /*    !__DC_TYPES_H__    */