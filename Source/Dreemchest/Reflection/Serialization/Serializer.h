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

#ifndef __DC_Reflection_Serializer_H__
#define __DC_Reflection_Serializer_H__

#include "../Reflection.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

    //! Uses embedded meta-object instance to serialize object properties to a key-value storage.
    class Serializer {
    public:

        virtual                 ~Serializer( void ) {}

        //! Writes an object instance to a key-value storage.
        virtual bool            serialize( InstanceConst instance, KeyValue& ar ) const;

        //! Reads an object instance from a key-value storage.
        virtual Instance        deserialize( AssemblyWPtr assembly, const KeyValue& ar );

    protected:

        //! Reads instance properties from a key-value storage.
        Instance                createAndDeserialize( AssemblyWPtr assembly, const String& name, const KeyValue& ar ) const;
    };

} // namespace Reflection

DC_END_DREEMCHEST

#endif    /*    !__DC_Reflection_Serializer_H__    */