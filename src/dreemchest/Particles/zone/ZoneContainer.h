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

#ifndef __DC_Particles_ZoneContainer_H__
#define __DC_Particles_ZoneContainer_H__

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

DC_BEGIN_DREEMCHEST

namespace Particles {

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

} // namespace Particles
    
DC_END_DREEMCHEST

#endif /*	!__DC_Particles_ZoneContainer_H__	*/