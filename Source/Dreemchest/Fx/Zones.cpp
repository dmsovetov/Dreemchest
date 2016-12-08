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
ZonePtr Zone::create( ZoneType type )
{
    switch( type ) {
    case ZoneDisk: return ZonePtr( DC_NEW DiskZone );
    case ZoneLine: return ZonePtr( DC_NEW LineZone );
    case ZoneBox:  return ZonePtr( DC_NEW BoxZone );
    }

    NIMBLE_BREAK_IF( true, "unhandled zone type" );
    return ZonePtr();
}

// ------------------------------------------------- DiskZone ------------------------------------------------- //

// ** DiskZone::DiskZone
DiskZone::DiskZone( f32 innerRadius, f32 outerRadius )
{
    m_innerRadius.setConstant( innerRadius );
    m_outerRadius.setConstant( outerRadius );
}

// ** DiskZone::type
ZoneType DiskZone::type( void ) const
{
    return ZoneDisk;
}

// ** DiskZone::generateRandomPoint
Zone::Point DiskZone::generateRandomPoint( f32 scalar, const Vec3& center ) const
{
    f32 inner = SampleParameter( 0, &m_innerRadius, 0.0f );
    f32 outer = SampleParameter( 0, &m_outerRadius, 0.0f );

    Vec2  direction = Vec2::randDirection();
    f32   distance  = RANDOM_SCALAR( inner, outer );

    return Point( center + Vec3( direction.x, direction.y, 0.0f ) * distance, Vec3( direction.x, direction.y, 0.0f ) );
}

// ------------------------------------------------- BoxZone ------------------------------------------------- //

// ** BoxZone::BoxZone
BoxZone::BoxZone( f32 width, f32 height, f32 depth )
{
    m_width.setConstant( width );
    m_height.setConstant( height );
    m_depth.setConstant( depth );
}

// ** BoxZone::type
ZoneType BoxZone::type( void ) const
{
    return ZoneBox;
}

// ** BoxZone::generateRandomPoint
Zone::Point BoxZone::generateRandomPoint( f32 scalar, const Vec3& center ) const
{
    f32 hw = SampleParameter( 0, &m_width,  0.0f ) * 0.5f;
    f32 hh = SampleParameter( 0, &m_height, 0.0f ) * 0.5f;
    f32 hd = SampleParameter( 0, &m_depth,  0.0f ) * 0.5f;

    Vec3 min( -hw, -hh, -hd );
    Vec3 max(  hw,  hh,  hd );

    return Point( center + randomValue( min, max ), Vec3( 0.0f, 1.0f, 0.0f ) );
}

// ------------------------------------------------- HemiSphereZone ------------------------------------------------- //

// ** HemiSphereZone::HemiSphereZone
HemiSphereZone::HemiSphereZone( f32 radius )
{
    m_radius.setConstant( radius );
}

// ** HemiSphereZone::type
ZoneType HemiSphereZone::type( void ) const
{
    return ZoneHemiSphere;
}

// ** HemiSphereZone::generateRandomPoint
Zone::Point HemiSphereZone::generateRandomPoint( f32 scalar, const Vec3& center ) const
{
    Vec3 direction = Vec3::randomHemisphereDirection( Vec3( 0.0f, 1.0f, 0.0f ) );
    return Point( center, direction );
}

// ------------------------------------------------- SphereZone ------------------------------------------------- //

// ** SphereZone::SphereZone
SphereZone::SphereZone( f32 radius )
{
    m_radius.setConstant( radius );
}

// ** SphereZone::type
ZoneType SphereZone::type( void ) const
{
    return ZoneSphere;
}

// ** SphereZone::generateRandomPoint
Zone::Point SphereZone::generateRandomPoint( f32 scalar, const Vec3& center ) const
{
    Vec3 direction = Vec3::randomDirection();
    f32  radius       = SampleParameter( 0, &m_radius, 0.0f );
    return Point( center + direction * radius, direction );
}

// ------------------------------------------------- LineZone ------------------------------------------------- //

// ** LineZone::LineZone
LineZone::LineZone( f32 length )
{
    m_length.setConstant( length );
}

// ** LineZone::type
ZoneType LineZone::type( void ) const
{
    return ZoneLine;
}

// ** LineZone::generateRandomPoint
Zone::Point LineZone::generateRandomPoint( f32 scalar, const Vec3& center ) const
{
    float length = SampleParameter( 0, &m_length, 0.0f );
    float angle  = SampleParameter( 0, &m_angle, 0.0f );

    Vec2  direction = Vec2::fromAngle( angle + 90.0f );
    f32   distance  = RANDOM_SCALAR( -length * 0.5f, length * 0.5f );

    return Point( center + Vec3( direction.x, direction.y, 0.0f ) * distance );
}

} // namespace Fx

DC_END_DREEMCHEST
