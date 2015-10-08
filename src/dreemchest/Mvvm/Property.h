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
	class GenericProperty : public IProperty {
    friend class Binding<TValue>;
	public:

		//! Alias the property type.
		typedef TValue				Type;

		//! Alias the strong pointer type.
		typedef StrongPtr< GenericProperty<TValue> > Ptr;

		//! Alias the weak pointer type.
		typedef WeakPtr< GenericProperty<TValue> > WPtr;

								    //! Constructs property.
								    GenericProperty( const DataWPtr& data, const TValue& value = TValue() )
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
        void                        subscribe( typename Binding<TValue>::WPtr binding );

        //! Unsubscribes from property changes.
        void                        unsubscribe( typename Binding<TValue>::WPtr binding );

        //! Notifies linked bindings about a property change.
        void                        notify( void );

	protected:

        //! Container type to store bindings.
        typedef Set<typename Binding<TValue>::WPtr>	Bindings;

		TValue						m_value;    //!< Actual property value.
        Bindings                    m_bindings; //!< Bindings that are linked to this property.
	};

	// ** GenericProperty::value
	template<typename TValue>
	const TValue& GenericProperty<TValue>::value( void ) const
	{
		return m_value;
	}

	// ** GenericProperty::value
	template<typename TValue>
	TValue& GenericProperty<TValue>::value( void )
	{
		return m_value;
	}

	// ** GenericProperty::set
	template<typename TValue>
	void GenericProperty<TValue>::set( const TValue& value )
	{
		if( value == m_value ) {
			return;
		}

		m_value = value;
        notify();
	}

	// ** GenericProperty::value
	template<typename TValue>
	TypeIdx GenericProperty<TValue>::type( void ) const
	{
		return GroupedTypeIndex<TValue, Property>::idx();
	}

    // ** GenericProperty::subscribe
    template<typename TValue>
    void GenericProperty<TValue>::subscribe( typename Binding<TValue>::WPtr binding )
    {
        m_bindings.insert( binding );
    }

    // ** GenericProperty::unsubscribe
    template<typename TValue>
    void GenericProperty<TValue>::unsubscribe( typename Binding<TValue>::WPtr binding )
    {
        m_bindings.erase( binding );
    }

    // ** GenericProperty::notify
    template<typename TValue>
    void GenericProperty<TValue>::notify( void )
    {
		if( m_data.valid() ) {
			m_data->check();
		}

        for( typename Bindings::iterator i = m_bindings.begin(), end = m_bindings.end(); i != end; ++i ) {
			typename Binding<TValue>::WPtr binding = *i;
            binding->handlePropertyChanged( m_value );
        }
    }

    //! Generic array property type.
    template<typename TValue>
    class GenericArrayProperty : public GenericProperty< Array<TValue> > {
    public:

								//! Constructs GenericArrayProperty
								GenericArrayProperty( const DataWPtr& data )
									: GenericProperty( data ) {}

        //! Pushes a new value to an array.
        void                    push( const TValue& value );

		//! Returns the array size.
		s32						size( void ) const;
    };

    // ** GenericArrayProperty::push
    template<typename TValue>
    void GenericArrayProperty<TValue>::push( const TValue& value )
    {
        this->m_value.push_back( value );
        this->notify();
    }

    // ** GenericArrayProperty::size
    template<typename TValue>
    s32 GenericArrayProperty<TValue>::size( void ) const
    {
		return this->m_value.size();
    }

} // namespace mvvm

DC_END_DREEMCHEST

#endif // __DC_Mvvm_Property_H__
