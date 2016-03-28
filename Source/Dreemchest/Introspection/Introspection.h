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

#ifndef __DC_Introspection_H__
#define __DC_Introspection_H__

#include "../Dreemchest.h"

DC_BEGIN_DREEMCHEST

namespace Introspection {

    NIMBLE_LOGGER_TAG( Intr )

    class Property;

    typedef void* Instance; //!< Instance is just a void pointer.

    //! Base class for meta object member types.
    class Member {
    public:

                                //! Constructs Member instance.
                                Member( CString name );
        virtual                 ~Member( void ) {}

        //! Returns the property name.
        CString                 name( void ) const;

        //! Perfors the type cast to a Property type.
        virtual const Property* asProperty( void ) const { return NULL; }
        virtual Property*       asProperty( void ) { return NULL; }

    private:

        CString                 m_name; //!< The property name.
    };

    //! The MetaObject class contains meta-information about objects.
    class MetaObject {
    public:

                            //! Constructs MetaObject instance.
                            MetaObject( CString name, Member** members, s32 memberCount );

                            //! Constructs MetaObject instance with a super class.
                            MetaObject( const MetaObject* super, CString name, Member** members, s32 memberCount );

        //! Returns the super class.
        const MetaObject*   super( void ) const;

        //! Returns the type name.
        CString             name( void ) const;

        //! Returns the total number of properties owned by introspected type.
        s32                 memberCount( void ) const;

        //! Returns the member at specified index.
        const Member*       member( s32 index ) const;
        Member*             member( s32 index );

        //! Returns the member with a specified name.
        const Member*       findMember( CString name ) const;
        Member*             findMember( CString name );

    private:

        const MetaObject*   m_super;        //!< Base class of an introspected type.
        CString             m_name;         //!< The introspected type name.
        Member**            m_members;      //!< All members exposed by data type.
        s32                 m_memberCount;  //!< The total number of members.
    };

    //! Forward declaration of a generic property type.
    template<typename TObject, typename TValue, typename TPropertyValue> class GenericProperty;

    //! Creates property that accepts POD values and is bound to a specified data type.
    template<typename TObject, typename TValue>
    Property* createProperty( CString name, TValue ( TObject::*getter )( void ) const, void ( TObject::*setter )( TValue ) )
    {
        return DC_NEW GenericProperty<TObject, TValue, TValue>( name, getter, setter );
    }

    //! Creates property that accepts refereces and is bound to a specified data type.
    template<typename TObject, typename TValue>
    Property* createProperty( CString name, const TValue& ( TObject::*getter )( void ) const, void ( TObject::*setter )( const TValue& ) )
    {
        return DC_NEW GenericProperty<TObject, TValue, const TValue&>( name, getter, setter );
    }

} // namespace Introspection

DC_END_DREEMCHEST

//! Embeds the virtual meta object accessor.
#define INSTROSPECTION_ACCESSOR( type ) \
            virtual const ::DC_DREEMCHEST_NS Introspection::MetaObject* metaObject( void ) const                    \
            {                                                                                                       \
                return type::staticMetaObject();                                                                    \
            }                                                                                                       \
            virtual ::DC_DREEMCHEST_NS Introspection::MetaObject* metaObject( void )                                \
            {                                                                                                       \
                return type::staticMetaObject();                                                                    \
            }

//! Embeds the instrospection as a static member.
#define INTROSPECTION( type, ... )                                                                                          \
            public:                                                                                                         \
            INSTROSPECTION_ACCESSOR( type )                                                                                 \
            static ::DC_DREEMCHEST_NS Introspection::MetaObject* staticMetaObject( void )                                   \
            {                                                                                                               \
                typedef type Object;                                                                                        \
                static ::DC_DREEMCHEST_NS Introspection::Member* m[] = { NULL, __VA_ARGS__ };                               \
                static ::DC_DREEMCHEST_NS Introspection::MetaObject meta( #type, m + 1, sizeof( m ) / sizeof( m[0] ) - 1 ); \
                return &meta;                                                                                               \
            }

//! Embeds the instrospection as a static member with a specified super class.
#define INTROSPECTION_SUPER( type, super, ... )                                                                                                         \
            public:                                                                                                                                     \
            INSTROSPECTION_ACCESSOR( type )                                                                                                             \
            static ::DC_DREEMCHEST_NS Introspection::MetaObject* staticMetaObject( void )                                                               \
            {                                                                                                                                           \
                typedef type Object;                                                                                                                    \
                static ::DC_DREEMCHEST_NS Introspection::Member* m[] = { NULL, __VA_ARGS__ };                                                           \
                static ::DC_DREEMCHEST_NS Introspection::MetaObject meta( super::staticMetaObject(), #type, m + 1, sizeof( m ) / sizeof( m[0] ) - 1 );  \
                return &meta;                                                                                                                           \
            }

//! Adds the property member to an introspection.
#define PROPERTY( name, getter, setter )    \
            ::DC_DREEMCHEST_NS Introspection::createProperty( #name, &Object::getter, &Object::setter ),

#ifndef DC_BUILD_LIBRARY
    #include "Property.h"
#endif  /*  !DC_BUILD_LIBRARY   */

#endif    /*    !__DC_Introspection_H__    */