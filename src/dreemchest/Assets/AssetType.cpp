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

// ** AssetType::Invalid
const AssetType AssetType::Invalid;

// ** AssetType::s_nameToType
Map<String, AssetType> AssetType::s_nameToType;

// ** AssetType::s_typeToName
Map<AssetType, String> AssetType::s_typeToName;

// ** AssetType::AssetType
AssetType::AssetType( void ) : m_type( ~0 )
{
}

// ** AssetType::AssetType
AssetType::AssetType( const AssetType& other ) : m_type( other.m_type )
{
}

// ** AssetType::AssetType
AssetType::AssetType( TypeIdx type ) : m_type( type )
{

}

// ** AssetType::operator ==
bool AssetType::operator == ( const AssetType& other ) const
{
    return m_type == other.m_type;
}

// ** AssetType::operator <
bool AssetType::operator < ( const AssetType& other ) const
{
    return m_type < other.m_type;
}

// ** AssetType::isValid
bool AssetType::isValid( void ) const
{
    return !(*this == Invalid);
}

// ** AssetType::bit
u32 AssetType::bit( void ) const
{
    DC_BREAK_IF( m_type > 31 );
    return 1 << BIT( m_type );
}

// ** AssetType::fromString
AssetType AssetType::fromString( const String& value )
{
    Map<String, AssetType>::const_iterator i = s_nameToType.find( value );
    
    if( i == s_nameToType.end() ) {
        DC_BREAK;
        return AssetType();
    }

    return i->second;
}

// ** AssetType::toString
String AssetType::toString( void ) const
{
    Map<AssetType, String>::const_iterator i = s_typeToName.find( m_type );

    if( i == s_typeToName.end() ) {
        DC_BREAK;
        return "";
    }

    return i->second;
}

} // namespace Assets

DC_END_DREEMCHEST