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

#ifndef __DC_Mvvm_Data_H__
#define __DC_Mvvm_Data_H__

#include "Property.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

    //! A base class for all view data providers.
    class Data : public RefCounted {
	friend class Property;
    public:

										Data( const String& name );
        virtual							~Data( void ) {}

		//! Runs the data validation routine.
		void							check( void );

		//! Returns the property by name.
		PropertyPtr						get( const String& name );

		//! Returns the data name.
		const String&					name( void ) const;

	protected:

		//! Returns true if the data is valid.
		virtual bool					validate( void ) const;

		//! Adds a new named property.
		template<typename TValue>
		GenericProperty<TValue>*		add( const String& name, const TValue& value = TValue() );

		//! Adds a new named array property.
		template<typename TValue>
		GenericArrayProperty<TValue>*	addArray( const String& name );

	protected:

		//! Named property container.
		typedef Map<String, PropertyPtr>	Properties;

		String							m_name;			//!< Data name.
		Properties						m_properties;	//!< All properties added to this data.
		mutable BoolProperty::Ptr		m_isValid;		//!< This property is updated by the data validation routine.
    };

	// ** Data::add
	template<typename TValue>
	GenericProperty<TValue>* Data::add( const String& name, const TValue& value )
	{
		GenericProperty<TValue>* prop = DC_NEW GenericProperty<TValue>( this, value );
		m_properties[name] = PropertyPtr( prop );
		return prop;
	}

	// ** Data::addArray
	template<typename TValue>
	GenericArrayProperty<TValue>* Data::addArray( const String& name )
	{
		GenericArrayProperty<TValue>* prop = DC_NEW GenericArrayProperty<TValue>( this );
		m_properties[name] = PropertyPtr( prop );
		return prop;
	}

	//! Generic data class.
	template<typename T>
	class GenericData : public Data {
	public:

		//! Alias the strong pointer type.
		typedef StrongPtr<T> Ptr;

		//! Alias the weak pointer type.
		typedef WeakPtr<T>	WPtr;

							//! Constructs GenericData instance.
							GenericData( void )
								: Data( TypeInfo<T>::name() ) {}

		//! Createa the RemoteHost data instance.
		template<typename ... Args>
		static StrongPtr<T> create( const Args& ... args )
		{
			return StrongPtr<T>( DC_NEW T( args... ) );
		}
	};

} // namespace mvvm
    
DC_END_DREEMCHEST

#endif // __DC_Mvvm_Data_H__
