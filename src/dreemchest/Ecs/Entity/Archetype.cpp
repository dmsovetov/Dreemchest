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

#include "Archetype.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

// ** ArchetypeBase::read
void ArchetypeBase::read( const Io::Storage* storage )
{
	Io::KeyValue bson;
	bson.read( storage );

	construct();
	setBson( bson );
}

// ** ArchetypeBase::write
void ArchetypeBase::write( Io::Storage* storage ) const
{
	bson().write( storage );
}

// ** ArchetypeBase::bson
Io::KeyValue ArchetypeBase::bson( void ) const
{
	Io::KeyValue result = Io::KeyValue::object();

	result["Type"] = typeName();
	result["_id"]  = id();

	const Components& items = components();

	for( Components::const_iterator i = items.begin(), end = items.end(); i != end; ++i ) {
		CString  key  = i->second->typeName();
		Io::KeyValue data = i->second->bson();

		if( data.isNull() ) {
			continue;
		}

		result[key] = data;
	}

	return result;
}

// ** ArchetypeBase::setBson
void ArchetypeBase::setBson( const Io::KeyValue& value )
{
	DC_BREAK_IF( value.get( "Type", "" ).asString() != typeName() );

	Components& items = components();

	// Load all added components
	for( Components::iterator i = items.begin(), end = items.end(); i != end; ++i ) {
		CString key = i->second->typeName();
		i->second->setBson( value.get( key ) );
	}
	
	// Create components from BSON
	const Io::KeyValue::Properties& kv = value.properties();

	for( Io::KeyValue::Properties::const_iterator i = kv.begin(); i != kv.end(); ++i ) {
		if( i->first == "Type" || i->first == "_id" ) {
			continue;
		}

		bool hasComponent = false;

		for( Components::iterator j = items.begin(); j != items.end(); ++j ) {
			if( j->second->typeName() == i->first ) {
				hasComponent = true;
				break;
			}
		}

		if( hasComponent ) {
			continue;
		}

		if( i->second.isNull() ) {
			continue;
		}

		ComponentPtr component = ecs()->createComponentByName( i->first, i->second );
		attachComponent( component.get() );
	}
}

} // namespace Ecs

DC_END_DREEMCHEST