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

#ifndef __DC_Fx_ParticleZones_H__
#define __DC_Fx_ParticleZones_H__

#include "../Parameter/Parameter.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

    // ** class Zone
    class Zone : public RefCounted {

		DC_DECLARE_IS( Zone, Zone, this );

    public:

        virtual             ~Zone( void ) {}

        static Zone*        create( ZoneType type );

        virtual ZoneType	type( void ) const												= 0;
        virtual Vec2        generateRandomPoint( float scalar, const Vec2& center ) const	= 0;
    };

    // ** class DiskZone
    class DiskZone : public Zone {
    public:

                            DiskZone( void );

        // ** Zone
        virtual ZoneType	type( void ) const { return ZoneDisk; }
        virtual Vec2        generateRandomPoint( float scalar, const Vec2& center ) const;

    private:

        Parameter           m_innerRadius;
        Parameter           m_outerRadius;
    };

    // ** class LineZone
    class LineZone : public Zone {
    public:

                            LineZone( void );

        // ** Zone
        virtual ZoneType	type( void ) const { return ZoneLine; }
        virtual Vec2        generateRandomPoint( float scalar, const Vec2& center ) const;

    private:

        Parameter           m_angle;
        Parameter           m_length;
    };

} // namespace Fx

DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_ParticleZones_H__	*/