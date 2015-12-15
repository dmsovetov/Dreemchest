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

#include "Storage.h"

DC_BEGIN_DREEMCHEST

namespace Io {

// ----------------------------------------BinaryStorage ------------------------------------------//

// ** BinaryStorage::position
s32 BinaryStorage::position( void ) const
{
	return m_stream->position();
}

// ** BinaryStorage::setPosition
void BinaryStorage::setPosition( s32 value, SeekOrigin origin ) const
{
	const_cast<BinaryStorage*>( this )->m_stream->setPosition( value, origin );
}

// ** BinaryStorage::bytesAvailable
s32 BinaryStorage::bytesAvailable( void ) const
{
	return m_stream->length() - m_stream->position();
}

// ** BinaryStorage::BinaryStorage
BinaryStorage::BinaryStorage( const StreamPtr& stream ) : m_stream( stream )
{
}

// ** BinaryStorage::read
void BinaryStorage::read( void* ptr, u32 size ) const
{
	m_stream->read( ptr, size );
}

// ** BinaryStorage::write
void BinaryStorage::write( const void* ptr, u32 size )
{
	m_stream->write( ptr, size );
}

// ** BinaryStorage::stream
StreamPtr BinaryStorage::stream( void ) const
{
	return m_stream;
}

// --------------------------------------- KeyValueStorage --------------------------------------- //

// ** KeyValueStorage::KeyValueStorage
KeyValueStorage::KeyValueStorage( Variant* value ) : m_root( value )
{

}

// ** KeyValueStorage::size
u32 KeyValueStorage::size( void ) const
{
	return m_root->type() == Variant::ArrayValue ? m_root->toArray().size() : 0;
}

// ** KeyValueStorage::write
void KeyValueStorage::write( const Key& key, const Variant& value )
{
	get( key ) = value;
}

// ** KeyValueStorage::read
Variant KeyValueStorage::read( const Key& key ) const
{
	return get( key );
}

// ** KeyValueStorage::object
KeyValueStoragePtr KeyValueStorage::object( const Key& key )
{
	get( key ) = Variant::RegistryValue;
	return KeyValueStoragePtr( DC_NEW KeyValueStorage( &get( key ) ) );
}

// ** KeyValueStorage::object
KeyValueStoragePtr KeyValueStorage::object( const Key& key ) const
{
	Variant& node = get( key );
	return node.type() == Variant::RegistryValue ? KeyValueStoragePtr( DC_NEW KeyValueStorage( &node ) ) : KeyValueStoragePtr();
}

// ** KeyValueStorage::array
KeyValueStoragePtr KeyValueStorage::array( const Key& key )
{
	get( key ) = Variant::ArrayValue;
	return KeyValueStoragePtr( DC_NEW KeyValueStorage( &get( key ) ) );
}

// ** KeyValueStorage::array
KeyValueStoragePtr KeyValueStorage::array( const Key& key ) const
{
	Variant& node = get( key );
	return node.type() == Variant::ArrayValue ? KeyValueStoragePtr( DC_NEW KeyValueStorage( &node ) ) : KeyValueStoragePtr();
}

// ** KeyValueStorage::value
Variant& KeyValueStorage::get( const Key& key ) const
{
	if( key.name ) {
		DC_BREAK_IF( m_root->type() != Variant::RegistryValue );
		return (*(m_root->toRegistry().get()))[key.name];
	}

	DC_BREAK_IF( m_root->type() != Variant::ArrayValue );
	VariantArray& array = const_cast<VariantArray&>( m_root->toArray() );

	if( key.index >= ( s32 )array.size() ) {
		array.resize( key.index + 1 );
	}

	return array[key.index];
}

// ---------------------------------------- JsonStorage ------------------------------------------ //

#ifdef HAVE_JSONCPP

// ** JsonStorage::JsonStorage
JsonStorage::JsonStorage( Json::Value* root ) : m_root( NULL ), m_isOwned( root == NULL )
{
	m_root = root ? root : DC_NEW Json::Value( Json::objectValue );
}

JsonStorage::~JsonStorage( void )
{
	if( m_isOwned ) delete m_root;
}

// ** JsonStorage::size
u32 JsonStorage::size( void ) const
{
	return m_root->size();
}

// ** JsonStorage::write
void JsonStorage::write( const Key& key, const Variant& value )
{
	switch( value.type() ) {
	case Variant::BooleanValue: get( key ) = Json::Value( value.toBool() );
								break;

	case Variant::NumberValue:	get( key ) = Json::Value( value.toDouble() );
								break;

	case Variant::StringValue:	get( key ) = Json::Value( value.toString() );
								break;

	default:					DC_BREAK;
	}
}

// ** JsonStorage::read
Variant JsonStorage::read( const Key& key ) const
{
	Json::Value& node = get( key );

	switch( node.type() ) {
	case Json::booleanValue:	return Variant( node.asBool() );
	case Json::realValue:		return Variant( node.asDouble() );
	case Json::stringValue:		return Variant( node.asString() );
	default:					DC_BREAK;
	}

	return Variant();
}

// ** JsonStorage::object
KeyValueStoragePtr JsonStorage::object( const Key& key )
{
	get( key ) = Json::objectValue;
	return KeyValueStoragePtr( DC_NEW JsonStorage( &get( key ) ) );
}

// ** JsonStorage::object
KeyValueStoragePtr JsonStorage::object( const Key& key ) const
{
	Json::Value& node = get( key );
	return node.isObject() ? KeyValueStoragePtr( DC_NEW JsonStorage( &node ) ) : KeyValueStoragePtr();
}

// ** JsonStorage::array
KeyValueStoragePtr JsonStorage::array( const Key& key )
{
	get( key ) = Json::arrayValue;
	return KeyValueStoragePtr( DC_NEW JsonStorage( &get( key ) ) );
}

// ** JsonStorage::array
KeyValueStoragePtr JsonStorage::array( const Key& key ) const
{
	Json::Value& node = get( key );
	return node.isArray() ? KeyValueStoragePtr( DC_NEW JsonStorage( &node ) ) : KeyValueStoragePtr();
}

// ** JsonStorage::value
Json::Value& JsonStorage::get( const Key& key ) const
{
	if( key.name ) {
		DC_BREAK_IF( !m_root->isObject() );
		return (*m_root)[key.name];
	}

	DC_BREAK_IF( !m_root->isArray() );
	return (*m_root)[key.index];
}

// ** JsonStorage::toString
String JsonStorage::toString( void ) const
{
	Json::StyledWriter writer;
	return writer.write( *m_root );
}

#endif

} // namespace Io

DC_END_DREEMCHEST
