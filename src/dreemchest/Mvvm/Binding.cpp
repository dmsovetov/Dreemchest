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
#include "Converter.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

IMPLEMENT_LOGGER( log )

// -------------------------------------------------- BindingFactory -------------------------------------------------- //

// ** BindingFactory::BindingFactory
BindingFactory::BindingFactory( void )
{
	registerConverter<GuidToTextConverter>();
	registerConverter<FloatToTextConverter>();
	registerConverter<IntegerToTextConverter>();
}

// ** BindingFactory::bindableValueTypes
ValueTypes BindingFactory::bindableValueTypes( void ) const
{
	ValueTypes result;

	for( Bindings::const_iterator i = m_bindings.begin(), end = m_bindings.end(); i != end; ++i ) {
		result.push_back( i->first );
	}

	return result;
}

// ** BindingFactory::create
BindingPtr BindingFactory::create( ValueTypeIdx valueType, WidgetTypeIdx widgetType, const String& widgetProperty )
{
	Bindings::iterator byValueType = m_bindings.find( valueType );

	if( byValueType == m_bindings.end() ) {
		return BindingPtr();
	}

	BindingByWidgetType::iterator byWidgetType = byValueType->second.find( widgetType );
	
	if( byWidgetType == byValueType->second.end() ) {
		return BindingPtr();
	}

	BindingByProperty::iterator byProperty = byWidgetType->second.find( widgetProperty );

	if( byProperty == byWidgetType->second.end() ) {
		return BindingPtr();
	}

	return byProperty->second->clone();
}

// ** BindingFactory::createConverter
BindingPtr BindingFactory::createConverter( ValueTypeIdx inputType, ValueTypeIdx outputType )
{
	ConvertersByInputType::iterator i = m_converters.find( inputType );

	if( i == m_converters.end() ) {
		return BindingPtr();
	}

	ConverterByOutputType::iterator j = i->second.find( outputType );

	if( j == i->second.end() ) {
		return BindingPtr();
	}

	return j->second->clone();
}

// ----------------------------------------------------- Bindings ------------------------------------------------------ //

// ** Bindings::Bindings
Bindings::Bindings( const BindingFactoryPtr& factory, const ObjectWPtr& root ) : m_factory( factory ), m_root( root )
{

}

// ** Bindings::setRoot
void Bindings::setRoot( const ObjectWPtr& value )
{
	m_root = value;
}

// ** Bindings::root
ObjectWPtr Bindings::root( void ) const
{
	return m_root;
}

//! Binds the widget to a value with specified URI.
bool Bindings::bind( const String& widget, const String& uri, ObjectWPtr root )
{
	if( !root.valid() ) {
		root = m_root;
	}

	ValueWPtr value = root->resolve( uri );
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
	WidgetTypeChain widgetType = resolveWidgetTypeChain( widgetName );
	Widget			widgetPtr  = findWidget( widgetName );

	return createBinding( value, widgetPtr, widgetType, widgetProperty );
/*
	if( !widgetPtr ) {
		log::error( "Bindings::bind : no widget with name '%s' found\n", widgetName.c_str() );
		return false;
	}

	// Create binding instance
	BindingPtr binding = m_factory->create( value->type(), widgetType[0], widgetProperty );

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

	return true;*/
}

// ** Bindings::createBinding
bool Bindings::createBinding( ValueWPtr value, Widget widget, const WidgetTypeChain& widgetType, const String& key )
{
	// Get the set of value types that can be bound.
	ValueTypes bindableTypes = m_factory->bindableValueTypes();

	// Push the actual property type to the begining.
	bindableTypes.insert( bindableTypes.begin(), value->type() );

	// For each widget type & value type pair try to create the binding - the first one wins.
	for( u32 i = 0, nwidgets = ( u32 )widgetType.size(); i < nwidgets; i++ ) {
		// Iterate over types
		for( u32 j = 0, ntypes = ( u32 )bindableTypes.size(); j < ntypes; j++ ) {
			ValueTypeIdx valueType = bindableTypes[j];
			BindingPtr	 binding   = m_factory->create( valueType, widgetType[i], key );

			if( !binding.valid() ) {
				continue;
			}

			// Binding found
			if( j == 0 ) {
				binding->bind( value, widget );
				binding->handleValueChanged();
				m_bindings.push_back( binding );
				return true;
			}

			// We have to create the converter.
			BindingPtr converter = m_factory->createConverter( value->type(), binding->type() );
		
			if( !converter.valid() ) {
				log::warn( "Bindings::createBinding : no converter found to bind the value to a widget.\n" );
				return false;
			}

			if( !converter->bind( value, NULL ) ) {
				log::warn( "Bindings::createBinding : failed to create converter.\n" );
				return false;				
			}

			if( !binding->bind( converter->converted(), widget ) ) {
				log::warn( "Bindings::createBinding : failed to bind to a converted value.\n" );
				return false;				
			}

			converter->handleValueChanged();
			binding->handleValueChanged();

			m_bindings.push_back( converter );
			m_bindings.push_back( binding );

			return true;
		}
	}

	return false;
}

} // namespace mvvm

DC_END_DREEMCHEST
