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

#include "Value.h"
#include "Binding.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

// ----------------------------------------------------------- Value ----------------------------------------------------------- //

// ** Value::setParent
void Value::setParent( ObjectWPtr value )
{
	m_parent = value;
}

// ** Value::is
bool Value::is( ValueTypeIdx expected ) const
{
	ValueTypeIdx actual = type();
	return actual == expected;
}

// ** Value::notifyValueChanged
void Value::notifyValueChanged( void )
{
	if( m_parent.valid() ) {
		m_parent->check();
	}

	for( Bindings::iterator i = m_bindings.begin(), end = m_bindings.end(); i != end; ++i ) {
		i->get()->handleValueChanged();
	}
}

// ** Value::addBinding
void Value::addBinding( BindingWPtr binding )
{
	m_bindings.insert( binding );
}

// ** Value::removeBinding
void Value::removeBinding( BindingWPtr binding )
{
	m_bindings.erase( binding );
}

// ----------------------------------------------------------- ObjectValue ----------------------------------------------------------- //

// ** ObjectValue::ObjectValue
ObjectValue::ObjectValue( void )
{
	m_isValid = value<Boolean>( "isValid", true );
	value<BoolCommand>( "save", dcThisMethod( ObjectValue::save ) );
}

// ** ObjectValue::isValid
const Boolean& ObjectValue::isValid( void ) const
{
	return *m_isValid;
}

// ** ObjectValue::save
bool ObjectValue::save( void )
{
	return false;
}

// ** ObjectValue::keys
Set<String> ObjectValue::keys( void ) const
{
	Set<String> result;

	for( Properties::const_iterator i = m_properties.begin(), end = m_properties.end(); i != end; ++i ) {
		result.insert( i->first );
	}

	return result;
}

// ** ObjectValue::is
bool ObjectValue::is( ValueTypeIdx expected ) const
{
	ValueTypeIdx actual = valueType<ObjectValue>();

	if( expected == actual ) {
		return true;
	}

	return Value::is( expected );
}

// ** ObjectValue::type
ValueTypeIdx ObjectValue::type( void ) const
{
	return Value::valueType<ObjectValue>();
}

// ** ObjectValue::validate
bool ObjectValue::validate( void ) const
{
	return true;
}

// ** ObjectValue::check
bool ObjectValue::check( void )
{
	bool result = validate();
	m_isValid->set( result );
	return result;
}

// ** ObjectValue::has
bool ObjectValue::has( const String& name ) const
{
	return m_properties.find( name ) != m_properties.end();
}

// ** ObjectValue::get
ValueWPtr ObjectValue::get( const String& name ) const
{
	Properties::const_iterator i = m_properties.find( name );
	return i != m_properties.end() ? i->second : ValuePtr();
}

// ** ObjectValue::set
void ObjectValue::set( const String& name, ValuePtr value )
{
	value->setParent( this );
	m_properties[name] = value;
}

// ** ObjectValue::resolve
ValueWPtr ObjectValue::resolve( const String& uri ) const
{
	u32 idx = uri.find( '.' );

	// No objects in URI - return the property.
	if( idx == String::npos ) {
		return get( uri );
	}

	// Split the URI.
	String data = uri.substr( 0, idx );
	String key  = uri.substr( idx + 1 );

	// Find the nested property.
	ValueWPtr value = get( data );

	// Cast the property to object.
	ObjectWPtr object = castTo<ObjectValue>( value );

	if( !object.valid() ) {
		return ValueWPtr();
	}

	return object->resolve( key );
}

#if DEV_DEPRECATED_KEYVALUE_TYPE

// ** ObjectValue::bson
KeyValue ObjectValue::bson( void ) const
{
	KeyValue result = KeyValue::object();

	for( Properties::const_iterator i = m_properties.begin(), end = m_properties.end(); i != end; ++i ) {
		const String& key = i->first;

		if( key == "isValid" ) {
			continue;
		}

		KeyValue value = i->second->bson();

		if( value.isNull() ) {
			continue;
		}

		result[key] = i->second->bson();
	}

	return result;
}

// ** ObjectValue::setBson
void ObjectValue::setBson( const KeyValue& value )
{
	const KeyValue::Properties& kv = value.properties();

	for( KeyValue::Properties::const_iterator i = kv.begin(), end = kv.end(); i != end; ++i ) {
		ValueWPtr value = get( i->first );

		if( value.valid() ) {
			value->setBson( i->second );
		}
	}
}

#else

// ** ObjectValue::bson
Variant ObjectValue::bson( void ) const
{
	KeyValue result;

	for( Properties::const_iterator i = m_properties.begin(), end = m_properties.end(); i != end; ++i ) {
		const String& key = i->first;

		if( key == "isValid" ) {
			continue;
		}

		Variant value = i->second->bson();

		if( !value.isValid() ) {
			continue;
		}

		result.setValueAtKey( key, value );
	}

	return Variant::fromValue( result );
}

// ** ObjectValue::setBson
void ObjectValue::setBson( const Variant& value )
{
    DC_NOT_IMPLEMENTED
}

#endif  /*  DEV_DEPRECATED_KEYVALUE_TYPE    */

// ----------------------------------------------------------- CommandValue ----------------------------------------------------------- //

// ** CommandValue::is
bool CommandValue::is( ValueTypeIdx expected ) const
{
	ValueTypeIdx actual = valueType<CommandValue>();

	if( expected == actual ) {
		return true;
	}

	return Value::is( expected );
}

// ** CommandValue::type
ValueTypeIdx CommandValue::type( void ) const
{
	return Value::valueType<CommandValue>();
}

#if DEV_DEPRECATED_KEYVALUE_TYPE
//! Returns the BSON object that represents this value.
KeyValue CommandValue::bson( void ) const
{
	return KeyValue::kNull;
}

//! Sets the BSON object that represents this value.
void CommandValue::setBson( const KeyValue& value )
{

}
#else
//! Returns the BSON object that represents this value.
Variant CommandValue::bson( void ) const
{
	return Variant();
}

//! Sets the BSON object that represents this value.
void CommandValue::setBson( const Variant& value )
{

}
#endif  /*  DEV_DEPRECATED_KEYVALUE_TYPE    */

// -------------------------------------------------------------- Command -------------------------------------------------------------- //

// ** Command::Command
Command::Command( const Callback& callback ) : m_callback( callback )
{

}

// ** Command::invoke
void Command::invoke( void )
{
	m_callback();
}

// ** Command::create
Command::Ptr Command::create( const Callback& callback )
{
	return Ptr( DC_NEW Command( callback ) );
}

} // namespace mvvm

DC_END_DREEMCHEST