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

#ifndef __DC_Reflection_MetaObject_H__
#define __DC_Reflection_MetaObject_H__

#include "../Reflection.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

    //! The MetaObject class contains meta-information about objects.
    class MetaObject {
    public:

                                //! Constructs MetaObject instance.
                                MetaObject( CString name, const Type* type );

        //! Returns the type name.
        CString                 name( void ) const;

        //! Returns the instrospected type instance.
        const Type*             type( void ) const;

        //! Returns an instance pointer type casted to Class or NULL, if the metaobject is not a class.
        virtual const Class*    isClass( void ) const { return NULL; }
        virtual Class*          isClass( void ) { return NULL; }

        //! Returns an instance pointer type casted to Enum or NULL, if the metaobject is not a class.
        virtual const Enum*     isEnum( void ) const { return NULL; }
        virtual Enum*           isEnum( void ) { return NULL; }

    private:

        CString                 m_name; //!< The type name.
        const Type*             m_type; //!< The introspected type instance.
    };

    namespace Private {

        //! Forward declaration of an Enum template class.
        template<typename T> class Enum;
    
        //! staticMetaObject function detector
        NIMBLE_DECLARE_MEMBER_DETECTOR( staticMetaObject )

        //! A helper type to detect meta-object associated with a class.
        struct ClassMetaObject {
            //! Returns the pointer to MetaObject instance embedded into a class.
            template<typename U>
            static NIMBLE_STATIC_IF( Has_staticMetaObject<U>, MetaObject* ) detectMetaObject( void )
            {
                return U::staticMetaObject();    
            }

            //! Returns the pointer to static enumeration meta-object.
            template<typename U>
            static NIMBLE_STATIC_IF( IsEnumClass<U>, MetaObject* ) detectMetaObject( void )
            {
                static Private::Enum<U> meta;
                return &meta;
            }

            //! Returns a NULL pointer if this is not an enum class and no staticMetaObject method found.
            template<typename U>
            static typename EnableIf<!IsEnumClass<U>::value && !Has_staticMetaObject<U>::value, MetaObject* >::value detectMetaObject( void )
            {
                return NULL;
            }
        };

        //! Returns NULL pointer for a primitive types.
        struct PrimitiveMetaObject {
            //! Just returns a NULL pointer for any primitive type.
            template<typename U>
            static MetaObject* detectMetaObject( void )
            {
                return NULL;
            }           
        };

    } // namespace Private

    //! Returns a pointer to a static MetaObject instance associated with a specified type.
    template<typename T>
    NIMBLE_INLINE MetaObject* staticMetaObject( void )
    {
        return TypeSelector<IsClassOrUnion<T>::value, Private::ClassMetaObject, Private::PrimitiveMetaObject>::type::detectMetaObject<T>();
    }

} // namespace Reflection

DC_END_DREEMCHEST

//! Embeds the virtual meta object accessor.
#define _INSTROSPECTION_ACCESSOR( type ) \
            virtual const ::DC_DREEMCHEST_NS Reflection::Class* metaObject( void ) const                            \
            {                                                                                                       \
                return type::staticMetaObject();                                                                    \
            }                                                                                                       \
            virtual ::DC_DREEMCHEST_NS Reflection::Class* metaObject( void )                                        \
            {                                                                                                       \
                return type::staticMetaObject();                                                                    \
            }                                                                                                       \
            virtual::DC_DREEMCHEST_NS Reflection:: MetaInstance metaInstance( void )                                \
            {                                                                                                       \
                return ::DC_DREEMCHEST_NS Reflection::MetaInstance( metaObject(), this );                           \
            }                                                                                                       \
            virtual ::DC_DREEMCHEST_NS Reflection::MetaInstanceConst metaInstance( void ) const                     \
            {                                                                                                       \
                return ::DC_DREEMCHEST_NS Reflection::MetaInstanceConst( metaObject(), this );                      \
            }

//! Embeds the instrospection as a static member.
#define INTROSPECTION( type, ... )                                                                                          \
            public:                                                                                                         \
            _INSTROSPECTION_ACCESSOR( type )                                                                                \
            static ::DC_DREEMCHEST_NS Reflection::Class* staticMetaObject( void )                                           \
            {                                                                                                               \
                DC_USE_DREEMCHEST                                                                                           \
                typedef type Object;                                                                                        \
                static Reflection::Member* m[] = { NULL, __VA_ARGS__ };                                                     \
                static Reflection::Class meta( #type, Type::fromClass<type>(), m + 1, sizeof( m ) / sizeof( m[0] ) - 1 );   \
                return &meta;                                                                                               \
            }

//! Embeds the interface instrospection as a static member.
#define INTROSPECTION_ABSTRACT( type, ... )                                                                                 \
            public:                                                                                                         \
            _INSTROSPECTION_ACCESSOR( type )                                                                                \
            static ::DC_DREEMCHEST_NS Reflection::Class* staticMetaObject( void )                                           \
            {                                                                                                               \
                DC_USE_DREEMCHEST                                                                                           \
                typedef type Object;                                                                                        \
                static Reflection::Member* m[] = { NULL, __VA_ARGS__ };                                                     \
                static Reflection::Class meta( #type, NULL, m + 1, sizeof( m ) / sizeof( m[0] ) - 1 );                      \
                return &meta;                                                                                               \
            }

//! Embeds the instrospection as a static member with a specified super class.
#define INTROSPECTION_SUPER( type, super, ... )                                                                             \
            public:                                                                                                         \
            _INSTROSPECTION_ACCESSOR( type )                                                                                \
            static ::DC_DREEMCHEST_NS Reflection::Class* staticMetaObject( void )                                           \
            {                                                                                                               \
                DC_USE_DREEMCHEST                                                                                           \
                typedef type Object;                                                                                        \
                static Reflection::Member* m[] = { NULL, __VA_ARGS__ };                                                     \
                static Reflection::Class meta( super::staticMetaObject(), #type, Type::fromClass<type>(), m + 1, sizeof( m ) / sizeof( m[0] ) - 1 ); \
                return &meta;                                                                                               \
            }

//! Adds the property member to an introspection.
#define PROPERTY( name, getter, setter, ... )    \
            Reflection::Private::createProperty( #name, &Object::getter, &Object::setter, Reflection::PropertyInfo( __VA_ARGS__ ) )

#endif    /*    !__DC_Reflection_MetaObject_H__    */