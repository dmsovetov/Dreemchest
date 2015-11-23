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

#include "Parameter.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

    //! Base class for zone types.
    class Zone : public RefCounted {
    public:

        virtual             ~Zone( void ) {}

		//! Creates the zone by type.
        static ZonePtr      create( ZoneType type );

		//! Returns the zone type.
        virtual ZoneType	type( void ) const											= 0;

		//! Generates the random point inside the zone.
        virtual Vec3        generateRandomPoint( f32 scalar, const Vec3& center ) const	= 0;
    };

    //! Disk zone generates points bewteen the inner and outer radii.
    class DiskZone : public Zone {
    public:

        //! Returns the zone type.
        virtual ZoneType	type( void ) const;

		//! Generates the random point inside the disk.
        virtual Vec3        generateRandomPoint( f32 scalar, const Vec3& center ) const;

    private:

        Parameter           m_innerRadius;	//!< Zone inner radius.
        Parameter           m_outerRadius;	//!< Zone outer radius.
    };

    //! Line zone generates points on a line segment.
    class LineZone : public Zone {
    public:

        //! Returns the zone type.
        virtual ZoneType	type( void ) const;

		//! Generates the random point on line segment.
        virtual Vec3        generateRandomPoint( f32 scalar, const Vec3& center ) const;

    private:

        Parameter           m_angle;	//!< Line segment rotation.
        Parameter           m_length;	//!< Line segment length.
    };

} // namespace Fx

DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_ParticleZones_H__	*/