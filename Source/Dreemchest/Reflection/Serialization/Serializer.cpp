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

#include "Serializer.h"

#include "../MetaObject/Assembly.h"
#include "../MetaObject/Class.h"
#include "../MetaObject/Property.h"
#include "../MetaObject/Instance.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

// ** Serializer::serialize
bool Serializer::serialize( InstanceConst instance, KeyValue& ar ) const
{
    DC_ABORT_IF( !instance, "invalid asset instance" );

    // Get the meta-class
    const Class* cls = instance.type();

    // Write instance type
    ar.setValueAtKey( "class", Variant::fromValue( cls->name() ) );

    // Write instance properties
    for( s32 i = 0, n = cls->memberCount(); i < n; i++ ) {
        // Get the class member at current index
        const Member* member = cls->member( i );

        // Is it a property?
        const Property* property = member->isProperty();

        // No it's not - just skip
        if( !property ) {
            continue;
        }

        // Is this property iterable?
        ConstIteratorUPtr iterator = property->iterator( instance );

        if( iterator.get() ) {
            serializeIterable( *cls, *property, instance, iterator, ar );
        } else {
            serializeValue( *cls, *property, instance, ar );
        }
    }

    return true;
}

// ** Serializer::deserialize
Instance Serializer::deserialize( AssemblyWPtr assembly, const KeyValue& ar )
{
    DC_ABORT_IF( !assembly.valid(), "invalid assembly" );

    // Read an instance type
    String name = ar.get<String>( "class" );

    // Create and read instance
    Instance instance = createAndDeserialize( assembly, name, ar );

    return instance;
}

// ** Serializer::createAndDeserialize
Instance Serializer::createAndDeserialize( AssemblyWPtr assembly, const String& name, const KeyValue& ar ) const
{
    // Construct class instance by name
    Instance instance = assembly->createInstance( name );

    if( !instance ) {
        LogError( "serializer", "unresolved class '%s'\n", name.c_str() );
        return instance;
    }

    // Read instance properties
    deserialize( instance, ar );

    return instance;
}

// ** Serializer::deserialize
void Serializer::deserialize( const Instance& instance, const KeyValue& ar ) const
{
    // Get the meta-class from instance
    const Class* cls = instance.type();

    // Now read instance properties
    for( s32 i = 0, n = cls->memberCount(); i < n; i++ ) {
        // Get the class member at current index
        const Member* member = cls->member( i );

        // Is it a property?
        const Property* property = member->isProperty();

        // No it's not - just skip
        if( !property ) {
            continue;
        }

        // Read a property value
        Variant value = readPropertyValue( cls, property, ar );

        // Value is invalid - just skip
        if( !value.isValid() ) {
            LogDebug( "serializer", "%s.%s does not exist inside a key-value storage\n", cls->name(), member->name() );
            continue;
        }

        // Is this property iterable?
        IteratorUPtr iterator = property->iterator( instance );

        if( iterator.get() ) {
            deserializeIterable( *cls, *property, instance, value, iterator );
        } else {
            deserializeValue( *cls, *property, instance, value );
        }
    }
}

// ** Serializer::deserializeIterable
void Serializer::deserializeIterable( const Class& cls, const Property& property, const Instance& instance, const Variant& value, IteratorUPtr iterator ) const
{
    // Get a list iterator
    ListIterator* list = iterator->isList();

    // Get an array value from a variant
    VariantArray array = value.as<VariantArray>();

    // Get an underlying container
    const VariantArray::Container& items = array;

    // Now insert elements to a container
    for( s32 i = 0, n = static_cast<s32>( items.size() ); i < n; i++ ) {
        // Get an element value
        const Variant& item = items[i];

        // Do we have to convert a value before sending it to a property?
        TypeConverter converter = findTypeConverter( item.type(), iterator->valueType() );

        // Perform a conversion and set a property
        list->insertAfter( converter ? converter( cls, property, item ) : item );   
    }
}

// ** Serializer::deserializeValue
void Serializer::deserializeValue( const Class& cls, const Property& property, const Instance& instance, const Variant& value ) const
{
    // Do we have to convert a value before sending it to a property?
    TypeConverter converter = findTypeConverter( value.type(), property.type() );

    // Perform a conversion and set a property
    if( converter ) {
        property.deserialize( instance, converter( cls, property, value ) );
    } else {
        property.deserialize( instance, value );
    }
}

// ** Serializer::serializeIterable
void Serializer::serializeIterable( const Class& cls, const Property& property, const InstanceConst& instance, ConstIteratorUPtr iterator, KeyValue& ar ) const
{
    // Create an empty variant array
    VariantArray array;

    // Iterate all elements and append them to an array
    while( iterator->next() ) {
        // Get current element
        Variant item = iterator->value();

        // Do we have to convert a value before writing it to a key-value archive?
        TypeConverter converter = findTypeConverter( item.type(), Type::fromClass<Variant>() );

        // Append an item to an array
        array << (converter ? converter( cls, property, item ) : item);
    }

    // Append a cunstructed array to a key-value storage
    ar.setValueAtKey( property.name(), Variant::fromValue( array ) );
}

// ** Serializer::serializeValue
void Serializer::serializeValue( const Class& cls, const Property& property, const InstanceConst& instance, KeyValue& ar ) const
{
    // Serialize property to a Variant
    Variant value = property.serialize( instance );

    // Do we have to convert a value before writing it to a key-value archive?
    TypeConverter converter = findTypeConverter( value.type(), Type::fromClass<Variant>() );

    // Perform a conversion
    if( converter ) {
        value = converter( cls, property, value );
    }

    // Now set a key inside a key-value archive
    ar.setValueAtKey( property.name(), value );
}

// ** Serializer::readPropertyValue
Variant Serializer::readPropertyValue( const Class* cls, const Property* property, const KeyValue& ar ) const
{
	DC_ABORT_IF( cls == NULL, "invalid class" );
	DC_ABORT_IF( property == NULL, "invalid property" );

	// First try to lookup value inside an archive
	const Variant& v = ar.valueAtKey( property->name() );

	if( v.isValid() ) {
		return v;
	}

	// No value inside an archive - try a default one
	PropertyDefaults::const_iterator i = m_defaults.find( calculatePropertyReaderHash( cls, property->name() ) );

	if( i != m_defaults.end() ) {
		return i->second( ar );
	}

	// Return a void value
	return Variant();
}

// ** Serializer::calculatePropertyReaderHash
String64 Serializer::calculatePropertyReaderHash( const Class* cls, CString name ) const
{
	return String64( (String( cls->name() ) + "." + name).c_str() );
}

// ** Serializer::calculateTypeConverterHash
u64 Serializer::calculateTypeConverterHash( const Type* from, const Type* to ) const
{
    return static_cast<u64>( from->id() ) << 32 | to->id();
}

// ** Serializer::findTypeConverter
Serializer::TypeConverter Serializer::findTypeConverter( const Type* from, const Type* to ) const
{
    // Calculate a hash value
    u64 hash = calculateTypeConverterHash( from, to );

    // Lookup a type converter
    TypeConverters::const_iterator i = m_typeConverters.find( hash );

    // Return a type converter
    return i != m_typeConverters.end() ? i->second : TypeConverter();
}

} // namespace Reflection

DC_END_DREEMCHEST