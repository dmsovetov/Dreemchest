//
//  Filename:   Parameter.cpp
//	Created:	28:05:2011   18:32

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include    "Parameter.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

// -------------------------------------------------------- Parameter -------------------------------------------------------- //

// ** Parameter::Parameter
Parameter::Parameter( void ) : m_mode( Constant ), m_type( Unknown ), m_scalar( NULL )
{
    m_isEnabled = false;

    setType( Scalar );
}

Parameter::~Parameter( void )
{
    switch( m_type ) {
    case Scalar: delete m_scalar;	break;
    case Color:  delete m_color;	break;
    default:						break;
    }
}

// ** Parameter::clear
void Parameter::clear( void )
{
    switch( m_type ) {
    case Scalar: m_scalar->curves[0].Clear();
                 m_scalar->curves[1].Clear();
                 break;
    case Color:  m_color->curves[0].Clear();
                 m_color->curves[1].Clear();
                 break;
    default:     DC_BREAK;
    }
}

// ** Parameter::sample
float Parameter::sample( float scalar, float defaultValue ) const
{
    DC_BREAK_IF( scalar < 0.0f || scalar > 1.0f );

    // ** Parameter type doesn't match a request
    if( m_type != Scalar ) {
        return defaultValue;
    }

    // ** Set result to a default value
    float result = defaultValue;

	switch( m_mode ) {
	case Constant:			{
								m_scalar->curves[0].Sample( 0, result );
							}
							break;
	case RandomConst:		{
								float a, b;
								m_scalar->curves[0].Sample( 0, a );
								m_scalar->curves[1].Sample( 0, b );

								result = RANDOM_SCALAR( a, b );
							}
							break;

	case Interpolate:		{
								m_scalar->curves[0].Sample( scalar, result );
							}
							break;

	case RandomInterpolate:	{
								float a, b;
								m_scalar->curves[0].Sample( scalar, a );
								m_scalar->curves[1].Sample( scalar, b );

								result = RANDOM_SCALAR( a, b );	
							}
							break;
	}

    return result;
}

// ** Parameter::sample
Rgb Parameter::sample( float scalar, const Rgb& defaultValue ) const
{
    DC_BREAK_IF( scalar < 0.0f || scalar > 1.0f );

    // ** Parameter type doesn't match a request
    if( m_type != Color ) {
        return defaultValue;
    }

    // ** Set result to a default value
    Rgb result = defaultValue;

	switch( m_mode ) {
	case Constant:			{
								m_color->curves[0].Sample( 0, result );
							}
							break;
	case RandomConst:		{
								Rgb a, b;
								m_color->curves[0].Sample( 0, a );
								m_color->curves[1].Sample( 0, b );

								result = RANDOM_COLOR( a, b );
							}
							break;

	case Interpolate:		{
								m_color->curves[0].Sample( scalar, result );
							}
							break;

	case RandomInterpolate:	{
								Rgb a, b;
								m_color->curves[0].Sample( scalar, a );
								m_color->curves[1].Sample( scalar, b );

								result = RANDOM_COLOR( a, b );	
							}
							break;
	}

    return result;
}

// ** Parameter::scalarCurve
CurveScalar* Parameter::scalarCurve( int curve ) const
{
    return m_type == Scalar ? &m_scalar->curves[curve] : NULL;
}

// ** Parameter::scalarCurve
CurveRgb* Parameter::colorCurve( int curve ) const
{
    return m_type == Color ? &m_color->curves[curve] : NULL;
}

// ** Parameter::setMode
void Parameter::setMode( eMode value )
{
	m_mode = value;
}

// ** Parameter::mode
Parameter::eMode Parameter::mode( void ) const
{
	return m_mode;
}

// ** Parameter::type
Parameter::eType Parameter::type( void ) const
{
    return m_type;
}

// ** Parameter::setType
void Parameter::setType( eType value )
{
    // ** The new type matches the previous one
    if( m_type == value ) {
        return;
    }

    // ** Delete previous data
    switch( m_type ) {
    case Scalar: delete m_scalar; break;
    case Color:  delete m_color;  break;
    default:                      break;
    }

    // ** Set the new type and create data
    m_type = value;

    switch( m_type ) {
    case Scalar: m_scalar = DC_NEW sScalarCurves;
				 m_scalar->curves[Min].Insert( 0, 0.0f, 0.0f );
				 m_scalar->curves[Min].Insert( 1, 1.0f, 0.0f );
				 m_scalar->curves[Max].Insert( 0, 0.0f, 0.0f );
				 m_scalar->curves[Max].Insert( 1, 1.0f, 0.0f );
				 break;

    case Color:  m_color = DC_NEW sColorCurves;
				 m_color->curves[Min].Insert( 0, 0.0f, White );
				 m_color->curves[Min].Insert( 1, 1.0f, White );
				 m_color->curves[Max].Insert( 0, 0.0f, White );
				 m_color->curves[Max].Insert( 1, 1.0f, White );
				 break;

    default:	 break;
    }
}

// ** Parameter::isEnabled
bool Parameter::isEnabled( void ) const
{
    return m_isEnabled;
}

// ** Parameter::setEnabled
void Parameter::setEnabled( bool value )
{
    m_isEnabled = value;
}

// ** Parameter::totalKeyframes
int Parameter::totalKeyframes( int curve ) const
{
    switch( m_type ) {
    case Scalar: return m_scalar->curves[curve].GetTotalKeyframes();
    case Color:  return m_color->curves[curve].GetTotalKeyframes();
    default:     break;
    }

    DC_BREAK;
    return 0;
}

// ** Parameter::setConstant
void Parameter::setConstant( float value )
{
	if( m_type != Scalar ) {
		return;
	}

	setMode( Constant );

	m_scalar->curves[Min].Clear();
	m_scalar->curves[Min].Insert( 0, 0.0f, value );
}

// ** Parameter::setRange
void Parameter::setRange( float min, float max )
{
	if( m_type != Scalar ) {
		return;
	}

	setMode( RandomConst );

	m_scalar->curves[Min].Clear();
	m_scalar->curves[Min].Insert( 0, 0.0f, min );

	m_scalar->curves[Max].Clear();
	m_scalar->curves[Max].Insert( 0, 0.0f, max );
}

// ** Parameter::data
FloatArray Parameter::data( int curve ) const
{
    FloatArray result;
    
    switch( m_type ) {
    case Scalar:    {
                        const CurveScalar& c = m_scalar->curves[curve];
        
                        for( int i = 0, n = c.GetTotalKeyframes(); i < n; i++ ) {
                            const CurveScalar::sKeyframe& kf = c.GetKeyframe( i );
                            result.push_back( kf.m_scalar );
                            result.push_back( kf.m_value );
                        }
                    }
                    break;

    case Color:     {
                        const CurveRgb& c = m_color->curves[curve];

                        for( int i = 0, n = c.GetTotalKeyframes(); i < n; i++ ) {
                            const CurveRgb::sKeyframe& kf = c.GetKeyframe( i );
                            result.push_back( kf.m_scalar );
                            result.push_back( kf.m_value.r );
                            result.push_back( kf.m_value.g );
                            result.push_back( kf.m_value.b );
                        }
                    }
                    break;

    default:        break;
    }

	return result;
}

// ** Parameter::setData
void Parameter::setData( const FloatArray& data, int curve )
{
    switch( m_type ) {
    case Scalar:    {
                        CurveScalar& c = m_scalar->curves[curve];
						const int    k = 2;

						c.Clear();
                        for( int i = 0, n = ( int )data.size() / k; i < n; i++ ) {
                            c.Insert( c.GetTotalKeyframes(), data[i * k + 0], data[i * k + 1] );
                        }
                    }
                    break;

    case Color:     {
                        CurveRgb& c = m_color->curves[curve];
						const int k = 4;

						c.Clear();
                        for( int i = 0, n = ( int )data.size() / k; i < n; i++ ) {
                            c.Insert( c.GetTotalKeyframes(), data[i * k + 0], Rgb( &data[i * k + 1] ) );
                        }
                    }
                    break;

    default:        break;
    }
}

} // namespace particles

} // namespace dreemchest