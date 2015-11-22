//
//  Filename:   ZoneContainer.h
//	Created:	28:05:2011   18:32

#ifndef		__DC_ZoneContainer_H__
#define		__DC_ZoneContainer_H__

/*
 =========================================================================================

        HEADERS & DEFS

 =========================================================================================
 */

#include    "../parameter/ParameterContainer.h"

/*
 =========================================================================================

        CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

    // ** struct sZoneInfo
    struct sZoneInfo {
        std::string         m_name;
        ZoneType            m_type;
    };

    // ** class ZoneContainer
    class ZoneContainer : public ParameterContainer {

        DC_DECLARE_IS( ZoneContainer, ZoneContainer, this );

        typedef std::vector<sZoneInfo> ZoneInfoArray;

    public:

                            ZoneContainer( void );
        virtual             ~ZoneContainer( void );

        int                 totalZones( void ) const;
        const sZoneInfo&    zoneInfo( int index ) const;
        Zone*               zone( void ) const;
        void                setZone( const char *name );
        void                setZone( ZoneType type );

    protected:

        void                registerZone( const char *name, ZoneType type );
        ZoneType            findZoneByName( const char *name ) const;

    private:

        ZoneInfoArray       m_zones;
        Zone*               m_zone;
    };

} // namespace particles
    
} // namespace dreemchest

#endif		/*	!__DC_ZoneContainer_H__	*/