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

#ifndef __DC_Fx_Emitter_H__
#define __DC_Fx_Emitter_H__

#include "Fx.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

	//! Particle emitter contains the emission zone & particle emission parameters. Contains an array of particles.
	class Emitter : public RefCounted {
	friend class ParticleSystem;
	public:

		//! Returns the particle emitter local position
		const Vec3&				position( void ) const;

		//! Sets the local position of a particle emitter.
		void					setPosition( const Vec3& value );

		//! Returns the total emission duration.
		f32						duration( void ) const;

		//! Sets the total emission duration.
		void					setDuration( f32 value );

		//! Returns the emitter name.
		const String&			name( void ) const;

		//! Sets the emitter name.
		void					setName( const String& value );

		//! Returns true if the particle emission is looped.
		bool					isLooped( void ) const;

		//! Sets the particle emission looping.
		void					setLooped( bool value );

		//! Returns the total number of particles inside this emitter.
		s32						particlesCount( void ) const;

		//! Returns the particles by index.
		ParticlesWPtr			particles( s32 index ) const;

		//! Returns the emission zone.
		ZoneWPtr				zone( void ) const;

		//! Sets the emission zone.
		void					setZone( const ZonePtr& value );

		//! Removes particles from emitter.
		void					removeParticles( const ParticlesWPtr& particles );

		//! Adds particles to an emitter.
		ParticlesWPtr			addParticles( void );

		//! Creates the particle emitter instance.
		EmitterInstancePtr		createInstance( void ) const;

	private:

								//! Constructs the Emitter instance.
								Emitter( void );

	private:

		ParticlesArray			m_particles;	//!< All particles that are emitted by this emitter.
		Vec3					m_position;		//!< Local particle emitter position.
		ZonePtr					m_zone;			//!< Particle emission zone.
		f32						m_duration;		//!< Total particle emission duration in seconds.
		bool					m_isLooped;		//!< Flag indicating that particle emission is looped.
		String					m_name;			//!< Particle emitter name.
	};

	//! Particle emitter instance.
	class EmitterInstance : public RefCounted {
	friend class Emitter;
	public:

		//! Returns the total number of alive particles.
		s32						aliveCount( void ) const;

		//! Returns true if the particle emission is ended.
		bool					hasEnded( void ) const;

		//! Performs the emitter instance update.
		s32						update( f32 dt, const Vec3& position );

		//! Performs the emitter warm up.
        void					warmUp( f32 dt, const Vec3& position );

	private:

								//! Constructs EmitterInstance instance.
								EmitterInstance( EmitterWPtr emitter );

		//! Updates the current emitter time.
		bool					updateTime( f32 dt );

	private:

		EmitterWPtr				m_emitter;		//!< Parent particle emitter.
		ParticlesInstancesArray	m_particles;	//!< Array of particles instances.
		f32						m_time;			//!< Current emitter time.
		s32						m_aliveCount;	//!< The total number of alive particles.
	};

} // namespace Fx

DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_Emitter_H__	*/