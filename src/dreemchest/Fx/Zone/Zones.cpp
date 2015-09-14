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

#include "Zones.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

// ------------------------------------------------- Zone ------------------------------------------------- //

// ** Zone::create
Zone* Zone::create( ZoneType type )
{
    switch( type ) {
    case ZoneType::None: return NULL;
    case ZoneType::Disk: return DC_NEW DiskZone;
    case ZoneType::Line: return DC_NEW LineZone;
    }

	DC_BREAK
    return NULL;
}

// ------------------------------------------------- DiskZone ------------------------------------------------- //

// ** DiskZone::DiskZone
DiskZone::DiskZone( void )
{
	registerParameter( "Inner", &m_innerRadius, sParameterInfo::Lifetime );
	registerParameter( "Outer", &m_outerRadius, sParameterInfo::Lifetime );
}

// ** DiskZone::generateRandomPoint
Vec2 DiskZone::generateRandomPoint( float scalar, const Vec2& center ) const
{
	float inner = SampleParameter( &m_innerRadius, 0.0f );
	float outer = SampleParameter( &m_outerRadius, 0.0f );

    Vec2  direction = Vec2::randDirection();
    float distance  = RANDOM_SCALAR( inner, outer );

    return center + direction * distance;
}

// ------------------------------------------------- LineZone ------------------------------------------------- //

// ** LineZone::LineZone
LineZone::LineZone( void )
{
	registerParameter( "Angle", &m_angle, sParameterInfo::Lifetime );
	registerParameter( "Length", &m_length, sParameterInfo::Lifetime );
}

// ** LineZone::generateRandomPoint
Vec2 LineZone::generateRandomPoint( float scalar, const Vec2& center ) const
{
	float length = SampleParameter( &m_length, 0.0f );
	float angle  = SampleParameter( &m_angle, 0.0f );

    Vec2  direction = Vec2::fromAngle( angle );
    float distance  = RANDOM_SCALAR( -length * 0.5f, length * 0.5f );

    return center + direction * distance;
}

} // namespace Fx

DC_END_DREEMCHEST
