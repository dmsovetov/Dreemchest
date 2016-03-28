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

#ifndef __DC_Introspection_Property_H__
#define __DC_Introspection_Property_H__

#include "Introspection.h"

DC_BEGIN_DREEMCHEST

namespace Introspection {

    //! The Property class provides meta-data about a property.
    class Property : public Member {
    public:

                                //! Constructs the Property instance.
                                Property( CString name, const Type& type )
                                    : Member( name ), m_type( type ) {}

        //! This type can be type casted to a property.
        virtual const Property* asProperty( void ) const DC_DECL_OVERRIDE { return this; }
        virtual Property*       asProperty( void ) DC_DECL_OVERRIDE { return this; }

        //! Returns the property value type.
        const Type&             type( void ) const { return m_type; }
    
        //! Sets the property value.
        virtual void            set( Instance instance, const Variant& value ) = 0;

        //! Gets the property value.
        virtual Variant         get( Instance instance ) const = 0;

    private:

        Type                    m_type; //!< The property value type.
    };

    //! Generic property bound to a specified type.
    template<typename TObject, typename TValue, typename TPropertyValue>
    class GenericProperty : public Property {
    public:

        //! The property getter.
        typedef TPropertyValue  ( TObject::*Getter )( void ) const;

        //! The property setter.
        typedef void            ( TObject::*Setter )( TPropertyValue );

                                //! Constructs the GenericProperty instance.
                                GenericProperty( CString name, Getter getter, Setter setter );

    protected:

        //! Sets the property value.
        virtual void            set( Instance instance, const Variant& value ) DC_DECL_OVERRIDE;

        //! Gets the property value.
        virtual Variant         get( Instance instance ) const DC_DECL_OVERRIDE;

    private:

        Getter                  m_getter;   //!< The property getter.
        Setter                  m_setter;   //!< The property setter.
    };

    // ** GenericProperty::GenericProperty
    template<typename TObject, typename TValue, typename TPropertyValue>
    GenericProperty<TObject, TValue, TPropertyValue>::GenericProperty( CString name, Getter getter, Setter setter )
        : Property( name, Type::fromValue<TValue>() ), m_getter( getter ), m_setter( setter )
    {
    }

    // ** GenericProperty::set
    template<typename TObject, typename TValue, typename TPropertyValue>
    void GenericProperty<TObject, TValue, TPropertyValue>::set( Instance instance, const Variant& value )
    {
        TValue v = value.as<TValue>();
        (reinterpret_cast<TObject*>( instance )->*m_setter)( v );
    }

    // ** GenericProperty::get
    template<typename TObject, typename TValue, typename TPropertyValue>
    Variant GenericProperty<TObject, TValue, TPropertyValue>::get( Instance instance ) const
    {
        TValue v = (reinterpret_cast<TObject*>( instance )->*m_getter)();
        return Variant::fromValue<TValue>( v );
    }

} // namespace Introspection

DC_END_DREEMCHEST

#endif    /*    !__DC_Introspection_Property_H__    */