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

#include "Data.h"
#include "View.h"

DC_BEGIN_DREEMCHEST

namespace mvvm {

// ** Data::Data
Data::Data( const String& name ) : m_name( name )
{
	m_isValid = DC_NEW BoolProperty( NULL, true );
}

// ** Data::isValid
BoolProperty::WPtr Data::isValid( void ) const
{
	return m_isValid;
}

// ** Data::name
const String& Data::name( void ) const
{
	return m_name;
}

// ** Data::validate
bool Data::validate( void ) const
{
	return true;
}

// ** Data::check
bool Data::check( void )
{
	bool result = validate();
	m_isValid->set( result );
	return result;
}

// ** Data::get
PropertyPtr Data::get( const String& name )
{
	if( name == "valid" ) {
		return m_isValid;
	}

	Properties::iterator i = m_properties.find( name );
	return i != m_properties.end() ? i->second : PropertyPtr();
}

} // namespace mvvm

DC_END_DREEMCHEST
