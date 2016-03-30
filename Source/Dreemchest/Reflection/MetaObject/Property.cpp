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

#include "Property.h"

DC_BEGIN_DREEMCHEST

namespace Reflection {

// -------------------------------------------------------------- PropertyInfo -------------------------------------------------------------- //

// ** PropertyInfo::PropertyInfo
PropertyInfo::PropertyInfo( void )
    : description( NULL )
{
}

// ** PropertyInfo::PropertyInfo
PropertyInfo::PropertyInfo( CString description )
    : description( description )
{
}

// ----------------------------------------------------------------- Property ----------------------------------------------------------------- //

// ** Property::Property
Property::Property( CString name, const Type* type, const PropertyInfo& info )
    : Member( name )
    , m_type( type )
    , m_info( info )
{
}

// ** Property::isProperty
const Property* Property::isProperty( void ) const
{
    return this;
}

// ** Property::isProperty
Property* Property::isProperty( void )
{
    return this;
}

// ** Property::type
const Type* Property::type( void ) const
{
    return m_type;
}

// ** Property::info
const PropertyInfo& Property::info( void ) const
{
    return m_info;
}

} // namespace Reflection

DC_END_DREEMCHEST