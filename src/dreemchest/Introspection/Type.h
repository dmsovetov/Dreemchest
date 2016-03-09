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

#ifndef __DC_Introspection_Type_H__
#define __DC_Introspection_Type_H__

#include "Introspection.h"

DC_BEGIN_DREEMCHEST

namespace Introspection {

    //! Introspection data type.
    class Type {
    public:

        //! Void data type constant.
        static const Type           Void;


                                    //! Constructs void data type.
                                    Type( void );

                                    //! Copies Type instance.
                                    Type( const Type& other );

        //! Compares two data types.
        bool                        operator == ( const Type& other ) const;

        //! Compares two data type.
        bool                        operator < ( const Type& other ) const;

        //! Returns the type size.
        s32                         size( void ) const;

        //! Returns true if this data type is not void.
        bool                        isValid( void ) const;

        //! Constructs the value instance.
        void                        construct( void* instance, const void* copy = NULL ) const;

        //! Returns true if the data type matches the specified class.
        template<typename TValue>
        bool                        is( void ) const;

        //! Returns the data type for specified type.
        template<typename TValue>
        static Type                 fromValue( void );

    private:

        //! Function pointer used for instance construction.
        typedef void ( *Constructor )( void* instance, const void* copy );

                                    //! Constructs Type instance.
                                    Type( TypeIdx type, s32 size, Constructor constructor );

    private:

        //! This genertic struct is used to initialize the data type constructors.
        template<typename TValue>
        struct GenericConstructor {
            //! Constructs an instance of type TValue.
            static void thunk( void* instance, const void* copy )
            {
                if( copy ) {
                    new( instance ) TValue( *reinterpret_cast<const TValue*>( copy ) );
                } else {
                    new( instance ) TValue;
                }
            }
        };

        TypeIdx                     m_type;         //!< Actual data type value.
        s32                         m_size;         //!< Value type size.
        Constructor                 m_constructor;  //!< Construction function.
    };

    // ** Type::fromValue
    template<typename TValue>
    Type Type::fromValue( void )
    {
        TypeIdx type = GroupedTypeIndex<TValue, Type>::idx();
        return Type( type, sizeof( TValue ), &GenericConstructor<TValue>::thunk );
    }

    // ** Type::is
    template<typename TValue>
    bool Type::is( void ) const
    {
        return *this == fromValue<TValue>();
    }

} // namespace Introspection

DC_END_DREEMCHEST

#endif  /*  !__DC_Introspection_Type_H__    */