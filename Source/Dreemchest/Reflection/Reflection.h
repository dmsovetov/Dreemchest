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

#ifndef __DC_Reflection_H__
#define __DC_Reflection_H__

#include "../Dreemchest.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

    NIMBLE_LOGGER_TAG( Reflection )

    class Instance;
        class InstanceConst;
    class MetaObject;
        class Class;
        class Enum;
    class Iterator;
        class ListIterator;
        class MapIterator;

    class Property;
    struct PropertyInfo;

    dcDeclarePtrs( Assembly )

    //! An iterator unique pointer type.
    typedef UPtr<Iterator>       IteratorUPtr;
    typedef UPtr<const Iterator> ConstIteratorUPtr;

} // namespace Reflection

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
    #include "MetaObject/Property.h"
    #include "MetaObject/Class.h"
    #include "MetaObject/Enum.h"
    #include "MetaObject/Assembly.h"
    #include "MetaObject/Instance.h"
    #include "Serialization/Serializer.h"
#endif  /*  !DC_BUILD_LIBRARY   */

#endif    /*    !__DC_Reflection_H__    */
