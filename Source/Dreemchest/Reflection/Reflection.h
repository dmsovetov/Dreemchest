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

    class MetaObject;
        class Class;
        class Enum;

    class Property;
    struct PropertyInfo;

    dcDeclarePtrs( Assembly )

    //! The MetaInstance class contains a pointer to an instance along with an associated meta-object.
    struct MetaInstance {
                                //! Constructs MetaInstance instance.
                                MetaInstance( void )
                                    : cls( NULL ), pointer( NULL ) {}

                                //! Constructs MetaInstance instance.
                                MetaInstance( const Class* cls, void* pointer )
                                    : cls( cls ), pointer( pointer ) {}

                                //! Returns true if this meta-instance is valid.
                                operator bool( void ) const { return pointer != NULL && cls != NULL; }

        const Class*            cls;        //!< Meta-class instance.
        void*                   pointer;    //!< Object instance pointer.
    };

    //! The MetaInstance class contains a pointer to an instance along with an associated meta-object.
    struct MetaInstanceConst {
                                //! Constructs MetaInstanceConst instance.
                                MetaInstanceConst( void )
                                    : cls( NULL ), pointer( NULL ) {}

                                //! Constructs MetaInstanceConst instance.
                                MetaInstanceConst( const Class* cls, const void* pointer )
                                    : cls( cls ), pointer( pointer ) {}

                                //! Constructs MetaInstanceConst instance from MetaInstance.
                                MetaInstanceConst( const MetaInstance& instance )
                                    : cls( instance.cls ), pointer( instance.pointer ) {}

                                //! Returns true if this meta-instance is valid.
                                operator bool( void ) const { return pointer != NULL && cls != NULL; }

        const Class*            cls;        //!< Meta-class instance.
        const void*             pointer;    //!< Object instance pointer.
    };

} // namespace Reflection

DC_END_DREEMCHEST

#ifndef DC_BUILD_LIBRARY
    #include "MetaObject/Property.h"
    #include "MetaObject/Class.h"
    #include "MetaObject/Enum.h"
    #include "Serialization/Serializer.h"
#endif  /*  !DC_BUILD_LIBRARY   */

#endif    /*    !__DC_Reflection_H__    */