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

#ifndef __DC_Mvvm_View_H__
#define __DC_Mvvm_View_H__

#include "Mvvm.h"
#include "Data.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

#if 0
    //! View is a base class for view renderers and is a composition of data providers and actions handlers.
    class View : public RefCounted {
    public:

		//! Adds a new event handler to this view.
		void											addHandler( const ActionHandlerPtr& handler );

	#ifndef DC_CPP11_DISABLED
		//! A generic method to add a new event handler to this view.
		template<typename TActionHandler, typename ... Args>
		void											addHandler( const Args& ... args );
	#endif	/*	!DC_CPP11_DISABLED	*/

		//! Adds a new data provider to this view.
		template<typename TData>
		void											addData( typename TData::Ptr data );

		//! Returns the data by type.
		template<typename TData>
		typename TData::WPtr							data( void ) const;

        //! Adds a new binding to view.
        void                                            addBinding( BindingPtr instance );

        //! Notifies action handler about an event.
        void                                            notify( const String& event );

        //! Clears the view data providers & handlers.
        void                                            clear( void );

	protected:

		//! Returns the property by a specified URI.
		PropertyPtr										findPropertyByUri( const String& uri );

		//! Returns the property by a specified URI and type.
		template<typename TValue>
		typename Property<TValue>::WPtr					findProperty( const String& uri );

    private:

        //! Data providers container.
		typedef Map<TypeIdx, DataPtr>					DataProviders;

		//! Name to data provider mapping.
		typedef Map<String, DataPtr>					DataProviderByName;

		//! An array of action handlers.
		typedef List<ActionHandlerPtr>					ActionHandlers;

		DataProviders			                        m_data;             //!< Data providers.
		DataProviderByName								m_dataByName;		//!< Data providers by name.
        ActionHandlers	                                m_actionHandlers;   //!< Action handlers.
		BindingsList				                    m_bindings;			//!< Active bindings.
    };

	// ** View::addData
	template<typename TData>
	void View::addData( typename TData::Ptr data )
	{
		data->check();

		TypeIdx idx = GroupedTypeIndex<TData, Data>::idx();
		m_data[idx] = data;
		m_dataByName[data->name()] = data;
	}

	// ** View::data
	template<typename TData>
	typename TData::WPtr View::data( void ) const
	{
		TypeIdx idx = GroupedTypeIndex<TData, Data>::idx();
		DataProviders::const_iterator i = m_data.find( idx );
		return i != m_data.end() ? typename TData::WPtr( static_cast<TData*>( i->second.get() ) ) : typename TData::WPtr();
	}

	// ** View::findProperty
	template<typename TValue>
	typename Property<TValue>::WPtr View::findProperty( const String& uri )
	{
		PropertyPtr property = findPropertyByUri( uri );

		if( !property.valid() ) {
			return typename Property<TValue>::WPtr();
		}

		typename Property<TValue>::WPtr result = castTo<TValue>( property );

		if( !result.valid() ) {
			log::error( "View::findProperty : failed to cast property '%s' to a requested type\n", uri.c_str() );
			return typename Property<TValue>::WPtr();
		}

		return result;
	}

#ifndef DC_CPP11_DISABLED
	// ** View::addHandler
	template<typename TActionHandler, typename ... Args>
	void View::addHandler( const Args& ... args )
	{
		addHandler( DC_NEW TActionHandler( this, args... ) );
	}
#endif	/*	!DC_CPP11_DISABLED	*/

#endif

} // namespace mvvm
    
DC_END_DREEMCHEST

#endif	/*	!__DC_Mvvm_View_H__	*/