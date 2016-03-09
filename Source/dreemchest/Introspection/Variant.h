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

#ifndef __DC_Introspection_Variant_H__
#define __DC_Introspection_Variant_H__

#include "Type.h"
#include "TypeCast.h"

DC_BEGIN_DREEMCHEST

namespace Introspection {

    //! The Variant class acts like a union for the most common data types.
    class Variant {
    public:

                        ~Variant( void );

                        //! Constructs Variant value as a copy of another one.
                        Variant( const Variant& other );

        //! Copies this Variant value from another one.
        const Variant&  operator = ( const Variant& other );

        //! Returns the value type.
        const Type&     type( void ) const;

        //! Casts the Value to a specified data type.
        template<typename TValue>
        TValue          as( void ) const;

        //! Constructs Value instance from data.
        template<typename TValue>
        static Variant  fromValue( const TValue& value );

    private:

                        //! Constructs Variant instance.
                        Variant( const Type& type, const void* copy );

        //! Returns the allocated data pointer.
        const void*     pointer( void ) const;
        void*           pointer( void );

        //! Allocates the data if the value size exceeds the maxium value.
        void*           allocate( void );

    private:

        //! The constant size for a static value buffer.
        enum { MaxValueSize = 16 };

        Type            m_type;                 //!< The stored value type.
        union {
            void*       m_pointer;              //!< Allocated data pointer.
            u8          m_value[MaxValueSize];  //!< Values whom size is less than the maximum one are stored here.
        };
    };

    // ** Variant::fromValue
    template<typename TValue>
    Variant Variant::fromValue( const TValue& value )
    {
        return Variant( Type::fromValue<TValue>(), &value );
    }

    // ** Variant::as
    template<typename TValue>
    TValue Variant::as( void ) const
    {
        TValue result;

        if( type().is<TValue>() ) {
            result = *reinterpret_cast<const TValue*>( pointer() );
        } else {
            bool success = TypeCast::cast( pointer(), m_type, result );
            DC_BREAK_IF( !success );
        }

        return result;
    }

} // namespace Introspection

DC_END_DREEMCHEST

#endif  /*  !__DC_Introspection_Variant_H__    */