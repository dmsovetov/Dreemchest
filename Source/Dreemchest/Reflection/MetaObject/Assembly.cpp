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

#include "Assembly.h"
#include "Instance.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

// ** Assembly::Assembly
Assembly::Assembly( void )
{
}

// ** Assembly::Assembly
Assembly::Assembly( const String& name, AssemblyWPtr parent )
    : m_name( name )
    , m_parent( parent )
{
}

// ** Assembly::create
AssemblyPtr Assembly::create( void )
{
    return DC_NEW Assembly;
}

// ** Assembly::create
AssemblyPtr Assembly::create( const String& name, AssemblyWPtr parent )
{
    return DC_NEW Assembly( name, parent );
}

// ** Assembly::createInstance
Instance Assembly::createInstance( const String& name ) const
{
    // Create a hash string from a requested class name.
    String64 hash( name.c_str() );

    // Find the class by hash value
    Classes::const_iterator i = m_classes.find( hash );

    if( i == m_classes.end() ) {
    //    LogError( "assembly", "unresolved class '%s'\n", name.c_str() );
        return Instance();
    }

    // Get the type instance being allocated
    const Type* type = i->second->type();

    // -------------------------------
    static bool warningShown = false;
    if( !warningShown ) {
        LogWarning( "assembly", "%s", "Assembly.cpp:81: this is just a prototyping code and it should not be used in production env\n" );
        warningShown = true;
    }

    // Allocate new instance
    void* pointer = new u8[type->size()];

    // Construct an instance
    type->construct( pointer );

    Instance instance( i->second, pointer );

    return instance;
}

} // namespace Reflection

DC_END_DREEMCHEST
