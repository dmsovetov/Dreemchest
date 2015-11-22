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

#include    "ParameterContainer.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

// ** ParameterContainer::totalParameters
int ParameterContainer::totalParameters( void ) const
{
	return ( int )m_parameters.size();
}

// ** ParameterContainer::parameterInfo
const sParameterInfo& ParameterContainer::parameterInfo( int index ) const
{
	DC_BREAK_IF( index < 0 || index >= totalParameters() );
	return m_parameters[index];
}

// ** ParameterContainer::findParameterByName
Parameter* ParameterContainer::findParameterByName( const char *name ) const
{
	for( int i = 0, n = totalParameters(); i < n; i++ ) {
		if( m_parameters[i].m_name == name ) {
			return m_parameters[i].m_parameter;
		}
	}

	return NULL;
}

// ** ParameterContainer::registerParameter
void ParameterContainer::registerParameter( const char *name, Parameter *parameter, u8 flags )
{
	DC_BREAK_IF( findParameterByName( name ) );
	
	sParameterInfo info;
	info.m_name = name;
	info.m_parameter = parameter;
	info.m_flags.On( flags );

	m_parameters.push_back( info );
}

} // namespace particles
    
} // namespace dreemchest