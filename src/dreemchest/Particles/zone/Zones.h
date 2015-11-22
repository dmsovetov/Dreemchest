//
//  Filename:   Zones.h
//	Created:	28:05:2011   18:32

#ifndef		__DC_ParticleZones_H__
#define		__DC_ParticleZones_H__

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

    // ** class Zone
    class Zone : public ParameterContainer {

		DC_DECLARE_IS( Zone, Zone, this );

    public:

        virtual             ~Zone( void ) {}

        static Zone*        create( ZoneType type );

        virtual ZoneType	type( void ) const                                         = 0;
        virtual vec2        generateRandomPoint( float scalar, const vec2& center ) const = 0;
    };

    // ** class DiskZone
    class DiskZone : public Zone {
    public:

                            DiskZone( void );

        // ** Zone
        virtual ZoneType	type( void ) const { return ZoneType::Disk; }
        virtual vec2        generateRandomPoint( float scalar, const vec2& center ) const;

    private:

        Parameter           m_innerRadius;
        Parameter           m_outerRadius;
    };

    // ** class LineZone
    class LineZone : public Zone {
    public:

                            LineZone( void );

        // ** Zone
        virtual ZoneType	type( void ) const { return ZoneType::Line; }
        virtual vec2        generateRandomPoint( float scalar, const vec2& center ) const;

    private:

        Parameter           m_angle;
        Parameter           m_length;
    };

} // namespace particles

} // namespace dreemchest

#endif		/*	!__DC_ParticleZones_H__	*/