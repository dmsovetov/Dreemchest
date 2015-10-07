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

#include "View.h"
#include "ActionHandler.h"
#include "Binding.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

IMPLEMENT_LOGGER( log )

// ** View::notify
void View::notify( const String& event )
{
    ActionHandlers::Parts& actionHandlers = m_actionHandlers.parts();

    for( ActionHandlers::Parts::iterator i = actionHandlers.begin(), end = actionHandlers.end(); i != end; ++i ) {
        i->second->handleEvent( event );
    }
}

// ** View::clear
void View::clear( void )
{
    m_bindings.clear();
    m_data.clear();
    m_actionHandlers.clear();
}

// ** View::addBinding
void View::addBinding( Binding* instance )
{
    instance->refreshView();
    m_bindings.push_back( instance );
}

// ** View::addData
void View::addData( const String& name, const DataPtr& data )
{
	m_data[name] = data;
}

// ** View::findProperty
PropertyPtr View::findProperty( const String& uri )
{
	u32 idx = uri.find( '.' );

	if( idx == String::npos ) {
		log::error( "View::findProperty : invalid URI passed, '%s'\n", uri.c_str() );
		return PropertyPtr();
	}

	// Split the URI
	String data = uri.substr( 0, idx );
	String key  = uri.substr( idx + 1 );

	// Find the data by name
	DataProviders::iterator i = m_data.find( data );

	if( i == m_data.end() ) {
		log::error( "View::findProperty : no data with name '%s'\n", data.c_str() );
		return PropertyPtr();
	}

	PropertyPtr result = i->second->get( key );

	if( !result.valid() ) {
		log::error( "View::findProperty : data '%s' has no property with name '%s'\n", data.c_str(), key.c_str() );
	}

	return result;
}

} // namespace mvvm

DC_END_DREEMCHEST
