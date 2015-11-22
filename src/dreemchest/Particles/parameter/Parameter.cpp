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

#include    "Parameter.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

DC_BEGIN_DREEMCHEST

namespace Particles {

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
    case Scalar: m_scalar->curves[0].clear();
                 m_scalar->curves[1].clear();
                 break;
    case Color:  m_color->curves[0].clear();
                 m_color->curves[1].clear();
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
								m_scalar->curves[0].sample( 0, result );
							}
							break;
	case RandomConst:		{
								float a, b;
								m_scalar->curves[0].sample( 0, a );
								m_scalar->curves[1].sample( 0, b );

								result = RANDOM_SCALAR( a, b );
							}
							break;

	case Interpolate:		{
								m_scalar->curves[0].sample( scalar, result );
							}
							break;

	case RandomInterpolate:	{
								float a, b;
								m_scalar->curves[0].sample( scalar, a );
								m_scalar->curves[1].sample( scalar, b );

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
								m_color->curves[0].sample( 0, result );
							}
							break;
	case RandomConst:		{
								Rgb a, b;
								m_color->curves[0].sample( 0, a );
								m_color->curves[1].sample( 0, b );

								result = RANDOM_COLOR( a, b );
							}
							break;

	case Interpolate:		{
								m_color->curves[0].sample( scalar, result );
							}
							break;

	case RandomInterpolate:	{
								Rgb a, b;
								m_color->curves[0].sample( scalar, a );
								m_color->curves[1].sample( scalar, b );

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
				 m_scalar->curves[Min].insert( 0, 0.0f, 0.0f );
				 m_scalar->curves[Min].insert( 1, 1.0f, 0.0f );
				 m_scalar->curves[Max].insert( 0, 0.0f, 0.0f );
				 m_scalar->curves[Max].insert( 1, 1.0f, 0.0f );
				 break;

    case Color:  m_color = DC_NEW sColorCurves;
				 m_color->curves[Min].insert( 0, 0.0f, Rgb( 1.0f, 1.0f, 1.0f ) );
				 m_color->curves[Min].insert( 1, 1.0f, Rgb( 1.0f, 1.0f, 1.0f ) );
				 m_color->curves[Max].insert( 0, 0.0f, Rgb( 1.0f, 1.0f, 1.0f ) );
				 m_color->curves[Max].insert( 1, 1.0f, Rgb( 1.0f, 1.0f, 1.0f ) );
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
    case Scalar: return m_scalar->curves[curve].keyframeCount();
    case Color:  return m_color->curves[curve].keyframeCount();
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

	m_scalar->curves[Min].clear();
	m_scalar->curves[Min].insert( 0, 0.0f, value );
}

// ** Parameter::setRange
void Parameter::setRange( float min, float max )
{
	if( m_type != Scalar ) {
		return;
	}

	setMode( RandomConst );

	m_scalar->curves[Min].clear();
	m_scalar->curves[Min].insert( 0, 0.0f, min );

	m_scalar->curves[Max].clear();
	m_scalar->curves[Max].insert( 0, 0.0f, max );
}

// ** Parameter::data
FloatArray Parameter::data( int curve ) const
{
    FloatArray result;
    
    switch( m_type ) {
    case Scalar:    {
                        const CurveScalar& c = m_scalar->curves[curve];
        
                        for( int i = 0, n = c.keyframeCount(); i < n; i++ ) {
                            const CurveScalar::Keyframe& kf = c.keyframe( i );
                            result.push_back( kf.m_time );
                            result.push_back( kf.m_value );
                        }
                    }
                    break;

    case Color:     {
                        const CurveRgb& c = m_color->curves[curve];

                        for( int i = 0, n = c.keyframeCount(); i < n; i++ ) {
                            const CurveRgb::Keyframe& kf = c.keyframe( i );
                            result.push_back( kf.m_time );
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

						c.clear();
                        for( int i = 0, n = ( int )data.size() / k; i < n; i++ ) {
                            c.insert( c.keyframeCount(), data[i * k + 0], data[i * k + 1] );
                        }
                    }
                    break;

    case Color:     {
                        CurveRgb& c = m_color->curves[curve];
						const int k = 4;

						c.clear();
                        for( int i = 0, n = ( int )data.size() / k; i < n; i++ ) {
                            c.insert( c.keyframeCount(), data[i * k + 0], Rgb( &data[i * k + 1] ) );
                        }
                    }
                    break;

    default:        break;
    }
}

} // namespace Particles

DC_END_DREEMCHEST