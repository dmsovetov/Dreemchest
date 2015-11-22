//
//  Filename:   ZoneContainer.cpp
//	Created:	28:05:2011   18:32

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

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