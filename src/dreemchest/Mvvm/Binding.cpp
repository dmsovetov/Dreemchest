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

#include "Binding.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

// -------------------------------------------------- BindingFactory -------------------------------------------------- //

// ** BindingFactory::create
BindingPtr BindingFactory::create( ValueTypeIdx valueType, WidgetPrototypeChain widgetPrototype, const String& widgetProperty )
{
	Bindings::iterator byValueType = m_bindings.find( valueType );

	if( byValueType == m_bindings.end() ) {
		return BindingPtr();
	}

	BindingByWidgetType::iterator byWidgetType = byValueType->second.end();

	while( byWidgetType == byValueType->second.end() && !widgetPrototype.empty() ) {
		byWidgetType = byValueType->second.find( widgetPrototype[0] );
		widgetPrototype.erase( widgetPrototype.begin() );
	} 
	
	if( byWidgetType == byValueType->second.end() ) {
		return BindingPtr();
	}

	BindingByProperty::iterator byProperty = byWidgetType->second.find( widgetProperty );

	if( byProperty == byWidgetType->second.end() ) {
		return BindingPtr();
	}

	return byProperty->second->clone();
}

// ----------------------------------------------------- Bindings ------------------------------------------------------ //

// ** Bindings::Bindings
Bindings::Bindings( const BindingFactoryPtr& factory, const ObjectWPtr& root ) : m_factory( factory ), m_root( root )
{

}

//! Binds the widget to a value with specified URI.
bool Bindings::bind( const String& widget, const String& uri )
{
	ValueWPtr value = m_root->resolve( uri );
	return bind( widget, value );
}

//! Binds the widget to a value.
bool Bindings::bind( const String& widget, const ValueWPtr& value )
{
	DC_BREAK_IF( !value.valid() );

	// Split the widget & it's property.
	String widgetName	  = widget;
	String widgetProperty = "";

	if( widget.find( '.' ) != String::npos ) {
		u32 idx = widget.find( '.' );
		widgetName	   = widget.substr( 0, idx );
		widgetProperty = widget.substr( idx + 1 );
	}

	// Get the widget value type and widget pointer.
	WidgetPrototypeChain widgetPrototype = resolveWidgetPrototypeChain( widgetName );
	Widget				 widgetPtr		 = findWidget( widgetName );

	if( !widgetPtr ) {
		log::error( "Bindings::bind : no widget with name '%s' found\n", widgetName.c_str() );
		return false;
	}

	// Create binding instance
	BindingPtr binding = m_factory->create( value->type(), widgetPrototype, widgetProperty );

	if( !binding.valid() ) {
		log::error( "Bindings::bind : do not know how to bind property to '%s'\n", widgetName.c_str() );
		return false;	
	}

	if( !binding->bind( value, widgetPtr ) ) {
		log::error( "Bindings::bind : failed to bind property to '%s'\n", widgetName.c_str() );
		return false;		
	}

	binding->handleValueChanged();
	m_bindings.push_back( binding );

	return true;
}

} // namespace mvvm

DC_END_DREEMCHEST
