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

#include "EntitySerializer.h"

#include "Entity/Entity.h"
#include "Component/Component.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

// ** Serializer::Serializer
Serializer::Serializer( EcsWPtr ecs, const Bitset& excluded )
    : m_ecs( ecs )
    , m_excluded( excluded )
{
    // Register an entity flags default value
    registerPropertyDefault<Entity>( "flags", dcThisMethod( Serializer::defaultEntityFlags ) );

    // Register entity value converter
    registerTypeConverter<Guid, EntityWPtr>( dcThisMethod( Serializer::convertGuidToEntity ) );
    registerTypeConverter<EntityWPtr, Variant>( dcThisMethod( Serializer::convertEntityToGuid ) );

    // ------------------------------------- workaround for strong pointers
    registerTypeConverter<Guid, EntityPtr>( dcThisMethod( Serializer::convertGuidToEntityPtr ) );
    registerTypeConverter<EntityPtr, Variant>( dcThisMethod( Serializer::convertEntityPtrToGuid ) );
}

// ** Serializer::serialize
bool Serializer::serialize( EntityWPtr entity, KeyValue& ar ) const
{
    // First serialize an entity itself
    if( !Reflection::Serializer::serialize( entity->metaInstance(), ar ) ) {
        return false;
    }

    // Get entity components
    const Entity::Components& components = entity->components();

    // Now serialize each entity component
    for( Entity::Components::const_iterator i = components.begin(), end = components.end(); i != end; i++ ) {
        // Skip excluded components
        if( m_excluded.is( i->second->typeIndex() ) ) {
            continue;
        }

        // Get a component meta-object
        const Reflection::Class* metaClass = i->second->metaObject();

        // Skip component with no type, this means it's an abstract data type
        if( !metaClass->type() ) {
            continue;
        }

        // Serialize component to a temporary key-value storage
        KeyValue component;
        Reflection::Serializer::serialize( i->second->metaInstance(), component );

        // Remove the 'class' field from a serialized data
        component.removeValueAtKey( "class" );

        // Append component to a root entity key-value container
        ar.setValueAtKey( i->second->metaObject()->name(), Variant::fromValue( component ) );
    }

    return true;
}

// ** Serializer::deserialize
bool Serializer::deserialize( Reflection::AssemblyWPtr assembly, EntityWPtr entity, const KeyValue& ar )
{
    // First deserialize an entity itself
    Reflection::Serializer::deserialize( entity->metaInstance(), ar );

    // Create components from key-value archive
    const KeyValue::Properties& kv = ar.properties();

    // Get the entity meta-class
    const Reflection::Class* metaObject = entity->metaObject();

    for( KeyValue::Properties::const_iterator i = kv.begin(); i != kv.end(); ++i ) {
        // Skip the class value & all entity properties
        if( i->first == "class" || metaObject->findMember( i->first.c_str() ) ) {
            continue;
        }

        // First lookup a component converter
        ComponentConverter converter = findComponentConverter( i->first );

        // Perform a conversion if converter found
        KeyValue kv;
        if( converter ) {
            kv = converter( i->second );
        } else {
            // Key-value storage expected
            if( !i->second.type()->is<KeyValue>() ) {
                LogError( "serializer", "entity component '%s' data is expected to be a key-value storage\n", i->first.c_str() );
                continue;
            }
            kv = i->second.as<KeyValue>();
        }

        // Create and read component instance
        Reflection::Instance instance = createAndDeserialize( assembly, i->first, kv );

        if( !instance ) {
            continue;
        }

        // Down-cast instance to an abstract component
        ComponentBase* component = instance.upCast<ComponentBase>();

        if( !component ) {
            LogError( "serializer", "'%s' is not a subclass of component\n", i->first.c_str() );
            continue;           
        }

        entity->attachComponent( component );
    }

    return true;
}

// ** Serializer::serializeComponent
void Serializer::serializeComponent( ComponentWPtr component, KeyValue& ar ) const
{
    Reflection::Serializer::serialize( component->metaInstance(), ar );
}

// ** Serializer::deserializeComponent
void Serializer::deserializeComponent( ComponentWPtr component, const KeyValue& ar ) const
{
    Reflection::Serializer::deserialize( component->metaInstance(), ar );
}

// ** Serializer::findComponentConverter
Serializer::ComponentConverter Serializer::findComponentConverter( const String& name ) const
{
    String32 hash( name.c_str() );
    ComponentConverters::const_iterator i = m_componentConverters.find( hash );
    return i != m_componentConverters.end() ? i->second : ComponentConverter();
}

// ** Serializer::defaultEntityFlags
Variant Serializer::defaultEntityFlags( const KeyValue& ar ) const
{
    return Variant::fromValue( 0 );
}

// ** Serializer::resolveEntity
EntityWPtr Serializer::resolveEntity( const Guid& id ) const
{
    return m_ecs->findEntity( id );
}

// ** Serializer::convertGuidToEntity
Variant Serializer::convertGuidToEntity( const Reflection::Class& cls, const Reflection::Property& property, const Variant& value ) const
{
    // Get a Guid value
    Guid id = value.as<Guid>();

    // Return a NULL pointer if this is an empty Guid
    if( id.isNull() ) {
        return Variant::fromValue( EntityWPtr() );
    }

    // Lookup entity by identifier
    EntityWPtr entity = resolveEntity( id );

    // Show a warning message if no entity found
    if( !entity.valid() ) {
        LogWarning( "serializer", "%s.%s, unresolved entity %s\n", cls.name(), property.name(), id.toString().c_str() );
    }

    return Variant::fromValue( entity );
}

// ** Serializer::convertEntityToGuid
Variant Serializer::convertEntityToGuid( const Reflection::Class& cls, const Reflection::Property& property, const Variant& value ) const
{
    // Get an entity reference
    EntityWPtr entity = value.as<EntityWPtr>();

    // Return entity id or a null guid as variant
    return Variant::fromValue( entity.valid() ? entity->id() : Guid() );
}

// ------------------------------------- workaround for strong pointers

// ** Serializer::convertGuidToEntityPtr
Variant Serializer::convertGuidToEntityPtr( const Reflection::Class& cls, const Reflection::Property& property, const Variant& value ) const
{
    // Get a Guid value
    Guid id = value.as<Guid>();

    // Return a NULL pointer if this is an empty Guid
    if( id.isNull() ) {
        return Variant::fromValue( EntityPtr() );
    }

    // Lookup entity by identifier
    EntityPtr entity = resolveEntity( id );

    // Show a warning message if no entity found
    if( !entity.valid() ) {
        LogWarning( "serializer", "%s.%s, unresolved entity %s\n", cls.name(), property.name(), id.toString().c_str() );
    }

    return Variant::fromValue( entity );
}

// ** Serializer::convertEntityPtrToGuid
Variant Serializer::convertEntityPtrToGuid( const Reflection::Class& cls, const Reflection::Property& property, const Variant& value ) const
{
    // Get an entity reference
    EntityPtr entity = value.as<EntityPtr>();

    // Return entity id or a null guid as variant
    return Variant::fromValue( entity.valid() ? entity->id() : Guid() );
}

} // namespace Ecs

DC_END_DREEMCHEST
