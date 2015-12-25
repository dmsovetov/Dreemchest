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

#include "Parameter.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

	//! Particle bursts structure.
	struct ParticleBurst {
		f32					time;			//!< The burst time.
		s32					count;			//!< The number of particles to burst.
		s32					lastIteration;	//!< Last iteration number the burst was emitted.
	};

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

		//! Returns particle emission parameter.
		const FloatParameter&	emission( void ) const;
		FloatParameter&			emission( void );

		//! Adds burst item.
		void					addBurst( f32 time, s32 count );

		//! Returns the total number of bursts.
		s32						burstCount( void ) const;

		//! Returns the burst by index.
		const ParticleBurst&	burst( s32 index ) const;

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

		//! Adds new emission module instance.
		void					addModule( const ModulePtr& module );

		//! Removes particles from emitter.
		void					removeParticles( const ParticlesWPtr& particles );

		//! Emits new particles.
		void					emit( Particle* particles, s32 first, s32 last, const Vec3& position, f32 time ) const;

		//! Adds particles to an emitter.
		ParticlesWPtr			addParticles( void );

		//! Creates the particle emitter instance.
		EmitterInstancePtr		createInstance( IMaterialFactoryWPtr materialFactory ) const;

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
		Array<ModulePtr>		m_modules;		//!< Modules used by this emitter to construct particles.
		FloatParameter			m_emission;		//!< Particle emission parameter.
		Array<ParticleBurst>	m_bursts;		//!< Particle bursts (additional to standard emission).
	};

	//! Particle emitter instance.
	class EmitterInstance : public RefCounted {
	friend class Emitter;
	public:

		//! Returns the total number of alive particles.
		s32						aliveCount( void ) const;

		//! Returns true if the particle emission is ended.
		bool					hasEnded( void ) const;

		//! Returns the total number of particles.
		s32						particlesCount( void ) const;

		//! Returns the particles instance.
		ParticlesInstanceWPtr	particles( s32 index ) const;

		//! Performs the emitter instance update.
		s32						update( f32 dt, const Vec3& position );

		//! Performs the emitter warm up.
        void					warmUp( f32 dt, const Vec3& position );

		//! Returns true if this emitter is now stopped.
		bool					isStopped( void ) const;

		//! Sets the stopped state of an emitter.
		void					setStopped( bool value );

	private:

								//! Constructs EmitterInstance instance.
								EmitterInstance( IMaterialFactoryWPtr materialFactory, EmitterWPtr emitter );

		//! Updates the current emitter time.
		bool					updateTime( f32 dt );

		//! Calculates the number of particles to emit.
		s32						calculateEmissionCount( f32 scalar, s32 maxCount );

	private:

		EmitterWPtr				m_emitter;			//!< Parent particle emitter.
		ParticlesInstancesArray	m_particles;		//!< Array of particles instances.
		f32						m_time;				//!< Current emitter time.
		f32						m_timeEmission;		//!< Emission timer.
		s32						m_aliveCount;		//!< The total number of alive particles.
		s32						m_iteration;		//!< Current iteration index.
		Array<ParticleBurst>	m_bursts;			//!< Particle bursts.
		bool					m_isStopped;		//!< Indicated that a particle emitter is stopped.
	};

} // namespace Fx

DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_Emitter_H__	*/