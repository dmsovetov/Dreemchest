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

#include "Variant.h"

DC_BEGIN_DREEMCHEST

namespace Introspection {

// ** Variant::Variant
Variant::Variant( const Type& type, const void* copy ) : m_type( type )
{
    m_type.construct( allocate(), copy );
}

// ** Variant::Variant
Variant::Variant( const Variant& other ) : m_type( other.type() )
{
    m_type.construct( allocate(), other.pointer() );
}

// ** Variant::Variant
Variant::~Variant( void )
{
    if( m_type.size() > MaxValueSize ) {
        free( m_pointer );
    }
}

// ** Variant::operator =
const Variant& Variant::operator = ( const Variant& other )
{
    m_type = other.type();
    m_type.construct( allocate(), other.pointer() );
    return *this;
}

// ** Variant::type
const Type& Variant::type( void ) const
{
    return m_type;
}

// ** Variant::allocate
void* Variant::allocate( void )
{
    DC_BREAK_IF( !type().isValid() );

    // Get the value type size
    s32 size = type().size();

    // The value fits the static buffer.
    if( size <= MaxValueSize ) {
        return m_value;
    }

    m_pointer = malloc( size );
    return m_pointer;
}

// ** Variant::pointer
const void* Variant::pointer( void ) const
{
    return const_cast<Variant*>( this )->pointer();
}

// **  Variant::pointer
void* Variant::pointer( void )
{
    if( m_type.size() > MaxValueSize ) {
        return m_pointer;
    }
    
    return m_value;
}

} // namespace Introspection

DC_END_DREEMCHEST