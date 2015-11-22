//
//  Filename:   Zones.cpp
//	Created:	28:05:2011   18:32

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include    "Zones.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

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
vec2 DiskZone::generateRandomPoint( float scalar, const vec2& center ) const
{
	float inner = SampleParameter( &m_innerRadius, 0.0f );
	float outer = SampleParameter( &m_outerRadius, 0.0f );

    vec2  direction = vec2::RandDirection();
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
vec2 LineZone::generateRandomPoint( float scalar, const vec2& center ) const
{
	float length = SampleParameter( &m_length, 0.0f );
	float angle  = SampleParameter( &m_angle, 0.0f );

    vec2  direction = vec2::FromAngle( angle );
    float distance  = RANDOM_SCALAR( -length * 0.5f, length * 0.5f );

    return center + direction * distance;
}

} // namespace particles

} // namespace dreemchest