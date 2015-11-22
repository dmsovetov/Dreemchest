//
//  Filename:   ParameterContainer.cpp
//	Created:	28:05:2011   18:32

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

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