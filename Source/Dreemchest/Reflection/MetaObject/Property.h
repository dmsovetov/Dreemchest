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

#ifndef __DC_Reflection_Property_H__
#define __DC_Reflection_Property_H__

#include "Class.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

    //! Contains property meta-information like description, serialization flags, min/max range etc.
    struct PropertyInfo {
        CString             description;    //!< Property description string.

                            //! Constructs an empty PropertyInfo instance.
                            PropertyInfo( void );

                            //! Construct PropertyInfo instance.
                            PropertyInfo( CString description );
    };

    //! The Property class provides meta-data about a property.
    class Property : public Member {
    public:

                                //! Constructs the Property instance.
                                Property( CString name, const Type* type, const MetaObject* metaObject, const PropertyInfo& info );

        //! This type can be type casted to a property.
        virtual const Property* isProperty( void ) const NIMBLE_OVERRIDE;
        virtual Property*       isProperty( void ) NIMBLE_OVERRIDE;

        //! Returns the property value type.
        const Type*             type( void ) const;

        //! Returns the property value meta-object.
        const MetaObject*       metaObject( void ) const;

        //! Returns property information.
        const PropertyInfo&     info( void ) const;
    
        //! Sets the property value.
        virtual void            set( MetaInstance instance, const Variant& value ) const NIMBLE_ABSTRACT;

        //! Sets the property value only if it will be changed.
        virtual bool            update( MetaInstance instance, const Variant& value ) const NIMBLE_ABSTRACT;

        //! Gets the property value.
        virtual Variant         get( MetaInstanceConst instance ) const NIMBLE_ABSTRACT;

    private:

        const MetaObject*       m_metaObject;   //!< The property value meta-object.
        const Type*             m_type;         //!< The property value type.
        PropertyInfo            m_info;         //!< Property information.
    };

    namespace Private {

        //! Generic property bound to a specified type.
        template<typename TObject, typename TValue, typename TPropertyValue>
        class Property : public ::DC_DREEMCHEST_NS Reflection::Property {
        public:

            //! The property getter.
            typedef TPropertyValue  ( TObject::*Getter )( void ) const;

            //! The property setter.
            typedef void            ( TObject::*Setter )( TPropertyValue );

                                    //! Constructs the Property instance.
                                    Property( CString name, Getter getter, Setter setter, const PropertyInfo& info );

        protected:

            //! Sets the property value.
            virtual void            set( MetaInstance instance, const Variant& value ) const NIMBLE_OVERRIDE;

            //! Sets the property value only if it will be changed.
            virtual bool            update( MetaInstance instance, const Variant& value ) const NIMBLE_OVERRIDE;

            //! Gets the property value.
            virtual Variant         get( MetaInstanceConst instance ) const NIMBLE_OVERRIDE;

        private:

            Getter                  m_getter;   //!< The property getter.
            Setter                  m_setter;   //!< The property setter.
        };

        // ** Property::Property
        template<typename TObject, typename TValue, typename TPropertyValue>
        Property<TObject, TValue, TPropertyValue>::Property( CString name, Getter getter, Setter setter, const PropertyInfo& info )
            : :: DC_DREEMCHEST_NS Reflection::Property( name, Type::fromClass<TValue>(), staticMetaObject<TValue>(), info )
            , m_getter( getter )
            , m_setter( setter )
        {
        }

        // ** Property::set
        template<typename TObject, typename TValue, typename TPropertyValue>
        void Property<TObject, TValue, TPropertyValue>::set( MetaInstance instance, const Variant& value ) const
        {
            TValue v = value.as<TValue>();
            (reinterpret_cast<TObject*>( instance.pointer )->*m_setter)( v );
        }

        // ** Property::update
        template<typename TObject, typename TValue, typename TPropertyValue>
        bool Property<TObject, TValue, TPropertyValue>::update( MetaInstance instance, const Variant& value ) const
        {
            TObject* object = reinterpret_cast<TObject*>( instance.pointer );

            TValue v = value.as<TValue>();
            TValue o = (object->*m_getter)();

            if( v == o ) {
                return false;
            }

            (object->*m_setter)( v );
            return true;
        }

        // ** Property::get
        template<typename TObject, typename TValue, typename TPropertyValue>
        Variant Property<TObject, TValue, TPropertyValue>::get( MetaInstanceConst instance ) const
        {
            TValue v = (reinterpret_cast<const TObject*>( instance.pointer )->*m_getter)();
            return Variant::fromValue<TValue>( v );
        }

    } // namespace Private

} // namespace Reflection

DC_END_DREEMCHEST

#endif    /*    !__DC_Reflection_Property_H__    */