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

#ifndef __DC_Introspection_TypeCast_H__
#define __DC_Introspection_TypeCast_H__

#include "Type.h"

DC_BEGIN_DREEMCHEST

namespace Introspection {

#if 0

    //! This class interface is used by Variant to cast values.
    class TypeCast {
    public:

        //! Takes the input value pointer and it's type, then tries to perform a type cast.
        template<typename TOutput>
        static bool     cast( const void* input, const Type& type, TOutput& output );

        //! Declares a new type cast.
        template<typename TTypeCast>
        static void     declare( void );

        //! Declares the two-side static cast.
        template<typename TFirst, typename TSecond>
        static void     declareStaticCast( void );

        //! Returns the input value type accepted for conversion.
        virtual Type    input( void ) const = 0;

        //! Returns the output value type after the conversion.
        virtual Type    output( void ) const = 0;

    protected:

        //! Takes two pointers to input and output values and performs the value type cast.
        virtual void    cast( const void* input, void* output ) = 0;

        //! Constructs the default type casts.
        static void     construct( void );

    private:

        //! Container type to store type casts by an output value type.
        typedef Map<Type, TypeCast*> ValueCast;

        //! Container type to store all registered type casts.
        typedef Map<Type, ValueCast> RegisteredTypeCasts;

        static RegisteredTypeCasts  s_typeCasts;    //!< All registered type casts.
        static bool                 s_constructed;  //!< Indicates that default type casts were constructed.
    };

    // ** TypeCast::cast
    template<typename TOutput>
    bool TypeCast::cast( const void* input, const Type& type, TOutput& output )
    {
        // Ensure that default type casts were constructed
        if( !s_constructed ) {
            construct();
        }

        // First locate the type cast by an input type
        RegisteredTypeCasts::iterator i = s_typeCasts.find( type );
        if( i == s_typeCasts.end() ) {
            return false;
        }

        // Now locate the type cast by an output type
        ValueCast::iterator j = i->second.find( Type::fromValue<TOutput>() );
        if( j == i->second.end() ) {
            return false;
        }

        j->second->cast( input, &output );
        return true;
    }

    // ** TypeCast::declare
    template<typename TTypeCast>
    static void TypeCast::declare( void )
    {
        TTypeCast* cast = new TTypeCast;

        Type input  = cast->input();
        Type output = cast->output();

        s_typeCasts[input][output] = new TTypeCast();
    }

    // ** TypeCast::declareStaticCast
    template<typename TFirst, typename TSecond>
    static void TypeCast::declareStaticCast( void )
    {
        declare<StaticTypeCast<TFirst, TSecond>>();
        declare<StaticTypeCast<TSecond, TFirst>>();
    }

    // ** TypeCast::construct
    inline void TypeCast::construct( void )
    {
        declareStaticCast<s32, f32>();
        s_constructed = true;
    }

    //! Generic type casting interface.
    template<typename TInput, typename TOutput>
    class GenericTypeCast : public TypeCast {
    public:

        //! Returns the input value type accepted for conversion.
        virtual Type    input( void ) const DC_DECL_OVERRIDE;

        //! Returns the output value type after the conversion.
        virtual Type    output( void ) const DC_DECL_OVERRIDE;
    };

    // ** GenericTypeCast::input
    template<typename TInput, typename TOutput>
    Type GenericTypeCast<TInput, TOutput>::input( void ) const
    {
        return Type::fromValue<TInput>();
    }

    // ** GenericTypeCast::output
    template<typename TInput, typename TOutput>
    Type GenericTypeCast<TInput, TOutput>::output( void ) const
    {
        return Type::fromValue<TOutput>();
    }

    //! Genertic static type casting interface.
    template<typename TInput, typename TOutput>
    class StaticTypeCast : public GenericTypeCast<TInput, TOutput> {
    protected:

        //! Performs the static type cast.
        virtual void    cast( const void* input, void* output ) DC_DECL_OVERRIDE;
    };

    // ** StaticTypeCast::cast
    template<typename TInput, typename TOutput>
    void StaticTypeCast<TInput, TOutput>::cast( const void* input, void* output )
    {
        *reinterpret_cast<TOutput*>( output ) = static_cast<TOutput>( *reinterpret_cast<const TInput*>( input ) );
    }

#endif

} // namespace Introspection

DC_END_DREEMCHEST

#endif  /*  !__DC_Introspection_TypeCast_H__    */