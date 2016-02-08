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

#include "AssetType.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

// ** Type::Invalid
const Type Type::Invalid;

// ** Type::s_nameToType
Map<String, Type> Type::s_nameToType;

// ** Type::s_typeToName
Map<Type, String> Type::s_typeToName;

// ** Type::Type
Type::Type( void ) : m_type( ~0 )
{
}

// ** Type::Type
Type::Type( const Type& other ) : m_type( other.m_type )
{
}

// ** Type::Type
Type::Type( TypeIdx type ) : m_type( type )
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
    return !(*this == Invalid);
}

// ** Type::bit
u32 Type::bit( void ) const
{
    DC_BREAK_IF( m_type > 31 );
    return 1 << BIT( m_type );
}

// ** Type::fromString
Type Type::fromString( const String& value )
{
    Map<String, Type>::const_iterator i = s_nameToType.find( value );
    
    if( i == s_nameToType.end() ) {
        DC_BREAK;
        return Type();
    }

    return i->second;
}

// ** Type::toString
String Type::toString( void ) const
{
    Map<Type, String>::const_iterator i = s_typeToName.find( m_type );

    if( i == s_typeToName.end() ) {
        DC_BREAK;
        return "";
    }

    return i->second;
}

} // namespace Assets

DC_END_DREEMCHEST