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

// ** Serializer::serialize
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

        // Key-value storage expected
        if( !i->second.type()->is<KeyValue>() ) {
            LogError( "serializer", "entity component '%s' data is expected to be a key-value storage\n", i->first.c_str() );
            continue;
        }

        // Create and read component instance
        Reflection::Instance instance = createAndDeserialize( assembly, i->first, i->second.as<KeyValue>() );

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

} // namespace Ecs

DC_END_DREEMCHEST