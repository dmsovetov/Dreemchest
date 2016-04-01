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
        const Member* member = cls->member( i );

        if( const Property* property = member->isProperty() ) {
            ar.setValueAtKey( member->name(), property->get( instance ) );
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
        LogError( "serializer", "failed to create instance of '%s'\n", name.c_str() );
        return instance;
    }

    // Get the meta-class from instance
    const Class* cls = instance.type();

    // Now read instance properties
    for( s32 i = 0, n = cls->memberCount(); i < n; i++ ) {
        const Member* member = cls->member( i );

        if( const Property* property = member->isProperty() ) {
            const Variant& value = ar.valueAtKey( member->name() );

            if( !value.isValid() ) {
                LogWarning( "serializer", "property '%s' of type '%s' does not exist inside a key-value storage\n", member->name(), cls->name() );
                continue;
            }

            property->set( instance, value );
        }
    }

    return instance;
}

} // namespace Reflection

DC_END_DREEMCHEST