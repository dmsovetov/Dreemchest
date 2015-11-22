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

#include    "ZoneContainer.h"

#include    "Zones.h"

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

// ** ZoneContainer::ZoneContainer
ZoneContainer::ZoneContainer( void ) : m_zone( NULL )
{

}

ZoneContainer::~ZoneContainer( void )
{
    DC_DELETE( m_zone );
}

// ** ZoneContainer::totalZones
int ZoneContainer::totalZones( void ) const
{
    return ( int )m_zones.size();
}

// ** ZoneContainer::zoneInfo
const sZoneInfo& ZoneContainer::zoneInfo( int index ) const
{
    DC_BREAK_IF( index < 0 || index >= totalZones() );
    return m_zones[index];
}

// ** ZoneContainer::zone
Zone* ZoneContainer::zone( void ) const
{
    return m_zone;
}

// ** ZoneContainer::setZone
void ZoneContainer::setZone( ZoneType type )
{
    DC_DELETE( m_zone );
    m_zone = Zone::create( type );
}

// ** ZoneContainer::setZone
void ZoneContainer::setZone( const char *name )
{
    DC_DELETE( m_zone );

    if( name == NULL ) {
        return;
    }

    setZone( findZoneByName( name ) );
}

// ** ZoneContainer::registerZone
void ZoneContainer::registerZone( const char *name, ZoneType type )
{
    DC_BREAK_IF( findZoneByName( name ) != ZoneType::None );

    sZoneInfo zone;
    zone.m_name = name;
    zone.m_type = type;

    m_zones.push_back( zone );
}

// ** ZoneContainer::findZoneByName
ZoneType ZoneContainer::findZoneByName( const char *name ) const
{
    for( int i = 0, n = totalZones(); i < n; i++ ) {
        if( m_zones[i].m_name == name ) {
            return m_zones[i].m_type;
        }
    }

    return ZoneType::None;
}

} // namespace particles
    
} // namespace dreemchest