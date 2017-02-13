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
#include "Iterator.h"
#include "instance.h"

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
        virtual const Property*     isProperty( void ) const NIMBLE_OVERRIDE;
        virtual Property*           isProperty( void ) NIMBLE_OVERRIDE;

        //! Returns the property value type.
        const Type*                 type( void ) const;

        //! Returns the property value meta-object.
        const MetaObject*           metaObject( void ) const;

        //! Returns property information.
        const PropertyInfo&         info( void ) const;

        //! Returns an iterator instance if a property is a collection.
        virtual ConstIteratorUPtr   iterator( InstanceConst instance ) const NIMBLE_ABSTRACT;
        virtual IteratorUPtr        iterator( Instance instance ) const NIMBLE_ABSTRACT;
    
        //! Sets the property value.
        virtual void                set( Instance instance, const Variant& value ) const NIMBLE_ABSTRACT;

        //! Sets the property value only if it will be changed.
        virtual bool                update( Instance instance, const Variant& value ) const NIMBLE_ABSTRACT;

        //! Gets the property value.
        virtual Variant             get( InstanceConst instance ) const NIMBLE_ABSTRACT;

        //! Converts Variant to a property value.
        virtual void                deserialize( Instance instance, const Variant& value ) const NIMBLE_ABSTRACT;

        //! Converts a property value to a Variant.
        virtual Variant             serialize( InstanceConst instance ) const NIMBLE_ABSTRACT;

    private:

        const MetaObject*           m_metaObject;   //!< The property value meta-object.
        const Type*                 m_type;         //!< The property value type.
        PropertyInfo                m_info;         //!< Property information.
    };

    namespace Private {

        //! Generic value property bound to a specified type.
        template<typename TObject, typename TValue, typename TPropertyValue>
        class GenericProperty : public Property {
        public:

            //! The property getter.
            typedef TPropertyValue  ( TObject::*Getter )( void ) const;

            //! The property setter.
            typedef void            ( TObject::*Setter )( TPropertyValue );

                                        //! Constructs the GenericProperty instance.
                                        GenericProperty( CString name, Getter getter, Setter setter, const PropertyInfo& info );

        protected:

            //! Returns an iterator instance if a property is a collection.
            virtual ConstIteratorUPtr   iterator( InstanceConst instance ) const NIMBLE_OVERRIDE;
            virtual IteratorUPtr        iterator( Instance instance ) const NIMBLE_OVERRIDE;

            //! Sets the property value.
            virtual void                set( Instance instance, const Variant& value ) const NIMBLE_OVERRIDE;

            //! Sets the property value only if it will be changed.
            virtual bool                update( Instance instance, const Variant& value ) const NIMBLE_OVERRIDE;

            //! Gets the property value.
            virtual Variant             get( InstanceConst instance ) const NIMBLE_OVERRIDE;

            //! Decodes property value from a Variant.
            virtual void                deserialize( Instance instance, const Variant& value ) const NIMBLE_OVERRIDE;

            //! Encodes property value to Variant.
            virtual Variant             serialize( InstanceConst instance ) const NIMBLE_OVERRIDE;

        private:

            Getter                      m_getter;       //!< The property getter.
            Setter                      m_setter;       //!< The property setter.
        };

        // ** GenericProperty::GenericProperty
        template<typename TObject, typename TValue, typename TPropertyValue>
        GenericProperty<TObject, TValue, TPropertyValue>::GenericProperty( CString name, Getter getter, Setter setter, const PropertyInfo& info )
            : Property( name, Type::fromClass<TValue>(), staticMetaObject<TValue>(), info )
            , m_getter( getter )
            , m_setter( setter )
        {
        }

        // ** GenericProperty::iterator
        template<typename TObject, typename TValue, typename TPropertyValue>
        ConstIteratorUPtr GenericProperty<TObject, TValue, TPropertyValue>::iterator( InstanceConst instance ) const
        {
            TPropertyValue collection = (instance.pointer<TObject>()->*m_getter)();
            return Private::createIterator<TValue>( collection );
        }

        // ** GenericProperty::iterator
        template<typename TObject, typename TValue, typename TPropertyValue>
        IteratorUPtr GenericProperty<TObject, TValue, TPropertyValue>::iterator( Instance instance ) const
        {
            TPropertyValue collection = (instance.pointer<TObject>()->*m_getter)();
            return Private::createIterator<TValue>( collection );
        }

        // ** GenericProperty::set
        template<typename TObject, typename TValue, typename TPropertyValue>
        void GenericProperty<TObject, TValue, TPropertyValue>::set( Instance instance, const Variant& value ) const
        {
            TValue v = value.as<TValue>();
            (instance.pointer<TObject>()->*m_setter)( v );
        }

        // ** GenericProperty::update
        template<typename TObject, typename TValue, typename TPropertyValue>
        bool GenericProperty<TObject, TValue, TPropertyValue>::update( Instance instance, const Variant& value ) const
        {
            TObject* object = instance.pointer<TObject>();

            TValue v = value.as<TValue>();
            TValue o = (object->*m_getter)();

            if( v == o ) {
                return false;
            }

            (object->*m_setter)( v );
            return true;
        }

        // ** GenericProperty::get
        template<typename TObject, typename TValue, typename TPropertyValue>
        Variant GenericProperty<TObject, TValue, TPropertyValue>::get( InstanceConst instance ) const
        {
            TValue v = (instance.pointer<TObject>()->*m_getter)();
            return Variant::fromValue<TValue>( v );
        }

        // ** GenericProperty::deserialize
        template<typename TObject, typename TValue, typename TPropertyValue>
        void GenericProperty<TObject, TValue, TPropertyValue>::deserialize( Instance instance, const Variant& value ) const
        {
            TValue v;
            v << value;
            (instance.pointer<TObject>()->*m_setter)( v );
        }

        // ** GenericProperty::serialize
        template<typename TObject, typename TValue, typename TPropertyValue>
        Variant GenericProperty<TObject, TValue, TPropertyValue>::serialize( InstanceConst instance ) const
        {
            Variant result;
            TValue v = (instance.pointer<TObject>()->*m_getter)();
            v >> result;
            return result;
        }

    } // namespace Private

} // namespace Reflection

DC_END_DREEMCHEST

#endif    /*    !__DC_Reflection_Property_H__    */
