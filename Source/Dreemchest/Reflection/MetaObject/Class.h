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

#ifndef __DC_Reflection_Class_H__
#define __DC_Reflection_Class_H__

#include "MetaObject.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

    //! Base class for meta object member types.
    class Member {
    public:

                                //! Constructs Member instance.
                                Member( CString name );
        virtual                 ~Member( void ) {}

        //! Returns the property name.
        CString                 name( void ) const;

        //! Perfors the type cast to a Property type.
        virtual const Property* isProperty( void ) const { return NULL; }
        virtual Property*       isProperty( void ) { return NULL; }

    private:

        CString                 m_name; //!< The property name.
    };

    //! The Class meta object contains meta-information about a single class.
    class Class : public MetaObject {
    public:

                                //! Constructs Class instance.
                                Class( CString name, const Type* type, Member** members, s32 memberCount );

                                //! Constructs Class instance with a super class.
                                Class( const Class* super, CString name, const Type* type, Member** members, s32 memberCount );

        //! Returns the super class.
        const Class*            super( void ) const;

        //! Returns the total number of properties owned by introspected type.
        s32                     memberCount( void ) const;

        //! Returns the member at specified index.
        const Member*           member( s32 index ) const;
        Member*                 member( s32 index );

        //! Returns the member with a specified name.
        const Member*           findMember( CString name ) const;
        Member*                 findMember( CString name );

        //! Returns an instance pointer type casted to Class or NULL, if the metaobject is not a class.
        virtual const Class*    isClass( void ) const NIMBLE_OVERRIDE;
        virtual Class*          isClass( void ) NIMBLE_OVERRIDE;

    private:

        const Class*            m_super;        //!< Base class of an introspected type.
        Member**                m_members;      //!< All members exposed by data type.
        s32                     m_memberCount;  //!< The total number of members.
    };

    namespace Private {

        //! Forward declaration of a generic property type.
        template<typename TObject, typename TValue, typename TPropertyValue> class Property;

        //! Creates property that accepts POD values and is bound to a specified data type.
        template<typename TObject, typename TValue>
        ::DC_DREEMCHEST_NS Reflection::Property* createProperty( CString name, TValue ( TObject::*getter )( void ) const, void ( TObject::*setter )( TValue ), const PropertyInfo& info )
        {
            return DC_NEW Property<TObject, TValue, TValue>( name, getter, setter, info );
        }

        //! Creates property that accepts refereces and is bound to a specified data type.
        template<typename TObject, typename TValue>
        ::DC_DREEMCHEST_NS Reflection::Property* createProperty( CString name, const TValue& ( TObject::*getter )( void ) const, void ( TObject::*setter )( const TValue& ), const PropertyInfo& info )
        {
            return DC_NEW Property<TObject, TValue, const TValue&>( name, getter, setter, info );
        }

    } // namespace Private

} // namespace Reflection

DC_END_DREEMCHEST

#endif    /*    !__DC_Reflection_Class_H__    */