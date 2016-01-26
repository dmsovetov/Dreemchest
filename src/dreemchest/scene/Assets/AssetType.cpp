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

namespace Scene {

// ** AssetType::Invalid
const AssetType AssetType::Invalid;

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

// ** AssetType::fromString
AssetType AssetType::fromString( const String& value )
{
    if( value == "Mesh" )       return fromClass<Mesh>();
    if( value == "Image" )      return fromClass<Image>();
    if( value == "Material" )   return fromClass<Material>();
    if( value == "Prefab" )     return fromClass<Prefab>();

    DC_BREAK;
    return AssetType();
}

// ** AssetType::toString
String AssetType::toString( void ) const
{
    if( is<Mesh>() )        return "Mesh";
    if( is<Image>() )       return "Image";
    if( is<Material>() )    return "Material";
    if( is<Prefab>() )      return "Prefab";

    DC_BREAK;
    return "";
}

} // namespace Scene

DC_END_DREEMCHEST