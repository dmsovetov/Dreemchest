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

#include "Type.h"
#include "TypeCast.h"

DC_BEGIN_DREEMCHEST

namespace Introspection {

#if 0

// ** TypeCast::s_typeCasts
TypeCast::RegisteredTypeCasts TypeCast::s_typeCasts;

// ** TypeCast::s_constructed
bool TypeCast::s_constructed = false;

// ** Type::Invalid
const Type Type::Void;

// ** Type::Type
Type::Type( void ) : m_type( ~0 )
{
}

// ** Type::Type
Type::Type( const Type& other ) : m_type( other.m_type ), m_size( other.m_size ), m_constructor( other.m_constructor )
{
}

// ** Type::Type
Type::Type( TypeIdx type, s32 size, Constructor constructor ) : m_type( type ), m_size( size ), m_constructor( constructor )
{

}

// ** Type::operator ==
bool Type::operator == ( const Type& other ) const
{
    return m_type == other.m_type;
}

// ** Type::operator <
bool Type::operator < ( const Type& other ) const
{
    return m_type < other.m_type;
}

// ** Type::isValid
bool Type::isValid( void ) const
{
    return !(*this == Void);
}

// ** Type::size
s32 Type::size( void ) const
{
    return m_size;
}

// ** Type::construct
void Type::construct( void* instance, const void* copy ) const
{
    DC_BREAK_IF( !m_constructor );
    m_constructor( instance, copy );
}

#endif

} // namespace Introspection

DC_END_DREEMCHEST