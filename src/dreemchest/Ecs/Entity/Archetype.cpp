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
void ArchetypeBase::read( const io::Storage* storage )
{
	io::Bson bson;
	bson.read( storage );

	construct();
	setBson( bson );
}

// ** ArchetypeBase::write
void ArchetypeBase::write( io::Storage* storage ) const
{
	bson().write( storage );
}

// ** ArchetypeBase::bson
io::Bson ArchetypeBase::bson( void ) const
{
	io::Bson result = io::Bson::object();

	result["Type"] = typeName();
	result["_id"]  = id();

	const Components& items = components();

	for( Components::const_iterator i = items.begin(), end = items.end(); i != end; ++i ) {
		CString key = i->second->typeName();
		result[key] = i->second->bson();
	}

	return result;
}

// ** ArchetypeBase::setBson
void ArchetypeBase::setBson( const io::Bson& value )
{
	DC_BREAK_IF( value.get( "Type", "" ).asString() != typeName() );

	Components& items = components();

	for( Components::iterator i = items.begin(), end = items.end(); i != end; ++i ) {
		CString key = i->second->typeName();
		i->second->setBson( value.get( key ) );
	}
}

} // namespace Ecs

DC_END_DREEMCHEST