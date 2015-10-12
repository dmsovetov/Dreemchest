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

#ifndef __DC_Mvvm_Property_H__
#define __DC_Mvvm_Property_H__

#include "Mvvm.h"
#include "Binding.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

	//! Property is an atomic item of data.
    class IProperty : public RefCounted {
    public:

								//! Constructs IProperty instance.
								IProperty( const DataWPtr& data )
									: m_data( data ) {}

        virtual                 ~IProperty( void ) {}

		//! Returns the property type id.
		virtual TypeIdx			type( void ) const { return 0; }

	protected:

		DataWPtr				m_data;	//!< Parent data instance.
    };

    //! Template class to simplify the property type declaration.
	template<typename TValue>
	class Property : public IProperty {
    friend class Binding<TValue>;
	public:

		typedef typename Binding<TValue>::WPtr	BindingType;	//!< Alias the binding type.
		typedef TValue							Type;			//!< Alias the property type.
		typedef StrongPtr< Property<TValue> >	Ptr;			//!< Alias the strong pointer type.
		typedef WeakPtr< Property<TValue> >		WPtr;			//!< Alias the weak pointer type.

								    //! Constructs Property instance.
								    Property( const DataWPtr& data, const TValue& value = TValue() )
									    : IProperty( data ), m_value( value ) {}

		//! Returns property value.
		const TValue&				value( void ) const;
		TValue&						value( void );

		//! Sets property value.
		void					    set( const TValue& value );

		//! Returns the property type id.
		virtual TypeIdx				type( void ) const;

    protected:

        //! Subscribes to property changes.
        void                        subscribe( BindingType binding );

        //! Unsubscribes from property changes.
        void                        unsubscribe( BindingType binding );

        //! Notifies linked bindings about a property change.
        void                        notify( void );

	protected:

        //! Container type to store bindings.
        typedef Set<BindingType>	Bindings;

		TValue						m_value;    //!< Actual property value.
        Bindings                    m_bindings; //!< Bindings that are linked to this property.
	};

	// ** Property::value
	template<typename TValue>
	const TValue& Property<TValue>::value( void ) const
	{
		return m_value;
	}

	// ** Property::value
	template<typename TValue>
	TValue& Property<TValue>::value( void )
	{
		return m_value;
	}

	// ** Property::set
	template<typename TValue>
	void Property<TValue>::set( const TValue& value )
	{
		if( value == m_value ) {
			return;
		}

		m_value = value;
        notify();
	}

	// ** Property::value
	template<typename TValue>
	TypeIdx Property<TValue>::type( void ) const
	{
		return GroupedTypeIndex<TValue, IProperty>::idx();
	}

    // ** Property::subscribe
    template<typename TValue>
    void Property<TValue>::subscribe( BindingType binding )
    {
        m_bindings.insert( binding );
    }

    // ** Property::unsubscribe
    template<typename TValue>
    void Property<TValue>::unsubscribe( BindingType binding )
    {
        m_bindings.erase( binding );
    }

    // ** Property::notify
    template<typename TValue>
    void Property<TValue>::notify( void )
    {
		if( m_data.valid() ) {
			m_data->check();
		}

        for( typename Bindings::iterator i = m_bindings.begin(), end = m_bindings.end(); i != end; ++i ) {
			BindingType binding = *i;
            binding->handlePropertyChanged( m_value );
        }
    }

	//! Generic data property type.
	template<typename TValue>
	class DataProperty : public Property< StrongPtr<Data> > {
	public:

								//! Constructs DataProperty instance.
								DataProperty( const DataWPtr& data, StrongPtr<TValue> value = StrongPtr<TValue>() )
									: Property( data, value ) {}

		//! Returns the casted property value.
		StrongPtr<TValue>		value( void ) const;
	};

	// ** DataProperty::value
	template<typename TValue>
	StrongPtr<TValue> DataProperty<TValue>::value( void ) const
	{
		return StrongPtr<TValue>( static_cast<TValue*>( m_value.get() ) );
	}

    //! Generic array property type.
    template<typename TValue>
    class ArrayProperty : public Property< Array<TValue> > {
    public:

								//! Constructs ArrayProperty
								ArrayProperty( const DataWPtr& data )
									: Property( data ) {}

        //! Pushes a new value to an array.
        void                    push( const TValue& value );

		//! Returns the array size.
		s32						size( void ) const;
    };

    // ** ArrayProperty::push
    template<typename TValue>
    void ArrayProperty<TValue>::push( const TValue& value )
    {
        this->m_value.push_back( value );
        this->notify();
    }

    // ** ArrayProperty::size
    template<typename TValue>
    s32 ArrayProperty<TValue>::size( void ) const
    {
		return this->m_value.size();
    }

} // namespace mvvm

DC_END_DREEMCHEST

#endif // __DC_Mvvm_Property_H__
