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

#ifndef __DC_Reflection_Enum_H__
#define __DC_Reflection_Enum_H__

#include "MetaObject.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

    //! Enum meta-object exposes fields of enumeration class declared with NIMBLE_DECLARE_ENUM class.
    class Enum : public MetaObject {
    public:

                            //! Constructs Enum instance.
                            Enum( CString name )
                                : MetaObject( name ) {}

        //! Returns the total number of entries inside this enumeration.
        virtual s32         size( void ) const NIMBLE_ABSTRACT;

        //! Returns the string value for an entry at specified index.
        virtual CString     stringAt( s32 index ) const NIMBLE_ABSTRACT;

        //! Returns the enum value of an entry at specified index.
        virtual s32         valueAt( s32 index ) const NIMBLE_ABSTRACT;

        //! Returns an instance pointer type casted to Class or NULL, if the metaobject is not a class.
        virtual const Enum* isEnum( void ) const NIMBLE_OVERRIDE { return this; }
        virtual Enum*       isEnum( void ) NIMBLE_OVERRIDE { return this; }
    };

    namespace Private {
    
        //! Generic enumeration class.
        template<typename TEnumClass>
        class Enum : public ::DC_DREEMCHEST_NS Reflection::Enum {
        public:

                                //! Constructs Enum instance.
                                Enum( void );

            //! Returns the total number of entries inside this enumeration.
            virtual s32         size( void ) const NIMBLE_OVERRIDE;

            //! Returns the string value for an entry at specified index.
            virtual CString     stringAt( s32 index ) const NIMBLE_OVERRIDE;

            //! Returns the enum value of an entry at specified index.
            virtual s32         valueAt( s32 index ) const NIMBLE_OVERRIDE;
        };

        // ** Enum::Enum
        template<typename TEnumClass>
        Enum<TEnumClass>::Enum( void )
            : ::DC_DREEMCHEST_NS Reflection::Enum( TEnumClass::name() )
        {
        }

        // ** Enum::valueAt
        template<typename TEnumClass>
        s32 Enum<TEnumClass>::size( void ) const
        {
            return TEnumClass::Total;
        }

        // ** Enum::valueAt
        template<typename TEnumClass>
        CString Enum<TEnumClass>::stringAt( s32 index ) const
        {
            return TEnumClass::stringAt( index );
        }

        // ** Enum::valueAt
        template<typename TEnumClass>
        s32 Enum<TEnumClass>::valueAt( s32 index ) const
        {
            return TEnumClass::valueAt( index );
        }

    } // namespace Private

} // namespace Reflection

DC_END_DREEMCHEST

#endif    /*    !__DC_Reflection_Enum_H__    */