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

#include "Emitter.h"

#include "Particles.h"
#include "Zones.h"
#include "Renderers.h"
#include "Modules.h"

#define EmitterParam( name ) m_parameter[name] ? &m_parameter[name] : NULL;

DC_BEGIN_DREEMCHEST

namespace Fx {

// ---------------------------------------- Emitter ---------------------------------------- //

// ** Emitter::Emitter
Emitter::Emitter( void ) : m_position( 0.0f, 0.0f, 0.0f ), m_duration( 1.0f ), m_isLooped( true )
{

}

// ** Emitter::position
const Vec3& Emitter::position( void ) const
{
	return m_position;
}

// ** Emitter::setPosition
void Emitter::setPosition( const Vec3& value )
{
	m_position = value;
}

// ** Emitter::duration
f32 Emitter::duration( void ) const
{
	return m_duration;
}

// ** Emitter::setDuration
void Emitter::setDuration( f32 value )
{
	m_duration = max2( value, 0.1f );
}

// ** Emitter::name
const String& Emitter::name( void ) const
{
	return m_name;
}

// ** Emitter::setName
void Emitter::setName( const String& value )
{
	m_name = value;
}

// ** Emitter::isLooped
bool Emitter::isLooped( void ) const
{
	return m_isLooped;
}

// ** Emitter::setLooped
void Emitter::setLooped( bool value )
{
	m_isLooped = value;
}

// ** Emitter::zone
ZoneWPtr Emitter::zone( void ) const
{
	return m_zone;
}

// ** Emitter::setZone
void Emitter::setZone( const ZonePtr& value )
{
	m_zone = value;
}

// ** Emitter::particlesCount
s32 Emitter::particlesCount( void ) const
{
	return static_cast<s32>( m_particles.size() );
}

// ** Emitter::particles
ParticlesWPtr Emitter::particles( s32 index ) const
{
	DC_BREAK_IF( index < 0 || index >= particlesCount() );
	return m_particles[index];
}

// ** Emitter::removeParticles
void Emitter::removeParticles( const ParticlesWPtr& particles )
{
	ParticlesArray::iterator i = std::find( m_particles.begin(), m_particles.end(), particles );

	if( i == m_particles.end() ) {
		DC_BREAK;
		return;
	}

	m_particles.erase( i );
}

// ** Emitter::addParticles
ParticlesWPtr Emitter::addParticles( void )
{
	Particles *particle = DC_NEW Particles( this );
	m_particles.push_back( particle );

	return particle;
}

// ** Emitter::addModule
void Emitter::addModule( const ModulePtr& module )
{
	DC_BREAK_IF( !module.valid() );
	LogDebug( "emitter", "module %s added\n", module->name().c_str() );
	m_modules.push_back( module );
}

// ** Emitter::burstCount
s32 Emitter::burstCount( void ) const
{
	return ( s32 )m_bursts.size();
}

// ** Emitter::burst
const ParticleBurst& Emitter::burst( s32 index ) const
{
	DC_BREAK_IF( index < 0 || index >= burstCount() );
	return m_bursts[index];
}

// ** Emitter::addBurst
void Emitter::addBurst( f32 time, s32 count )
{
	ParticleBurst burst;
	burst.time			= time;
	burst.count			= count;
	burst.lastIteration	= -1;
	m_bursts.push_back( burst );
}

// ** Emitter::emit
void Emitter::emit( Particle* particles, s32 first, s32 last, const Vec3& position, f32 time ) const
{
	// Construct the simulation state instance
	SimulationState state;
	state.m_direction = Vec3( 0.0f, 0.0f, 0.0f );
	state.m_dt		  = 0.0f;
	state.m_time	  = time;

	// Zero all particle data
	memset( particles->indices + first, 0, sizeof( particles->indices[0] ) * (last - first) );
	memset( particles->position + first, 0, sizeof( particles->position[0] ) * (last - first) );
	memset( particles->rotation + first, 0, sizeof( particles->rotation[0] ) * (last - first) );
	memset( particles->life + first, 0, sizeof( particles->life[0] ) * (last - first) );
	memset( particles->size + first, 0, sizeof( particles->size[0] ) * (last - first) );
	memset( particles->transparency + first, 0, sizeof( particles->transparency[0] ) * (last - first) );
	memset( particles->color + first, 0, sizeof( particles->color[0] ) * (last - first) );
	memset( particles->angularVelocity + first, 0, sizeof( particles->angularVelocity[0] ) * (last - first) );
	memset( particles->force + first, 0, sizeof( particles->force[0] ) * (last - first) );

	// Initialize particle positions & initial velocities
	Vec3*			 positions = particles->position;
	Particle::Force* forces	   = particles->force;
	u32*			 indices   = particles->indices;

	for( s32 i = first; i < last; i++ ) {
		Zone::Point point	= m_zone.valid() ? m_zone->generateRandomPoint( time, position ) : Zone::Point( position, Vec3( 0.0f, -1.0f, 0.0f ) );
		positions[i]		= point.position;
		forces[i].velocity	= point.direction;
		indices[i]			= rand() % FloatParameter::LifetimeRandomizationCount;
	}

	// Run all emission modules
	for( s32 i = 0, n = ( s32 )m_modules.size(); i < n; i++ ) {
		m_modules[i]->update( particles, first, last, state );
	}
}

// ** Emitter::emission
const FloatParameter& Emitter::emission( void ) const
{
	return m_emission;
}

// ** Emitter::emission
FloatParameter& Emitter::emission( void )
{
	return m_emission;
}

// ** Emitter::createInstance
EmitterInstancePtr Emitter::createInstance( IMaterialFactoryWPtr materialFactory ) const
{
	return DC_NEW EmitterInstance( materialFactory, const_cast<Emitter*>( this ) );
}

// ------------------------------------- EmitterInstance ---------------------------------------- //

// ** EmitterInstance::EmitterInstance
EmitterInstance::EmitterInstance( IMaterialFactoryWPtr materialFactory, EmitterWPtr emitter )
	: m_emitter( emitter ), m_time( 0.0f ), m_timeEmission( 0.0f ), m_aliveCount( 0 ), m_iteration( 0 ), m_isStopped( false )
{
	// Construct nested particles
	for( int i = 0, n = m_emitter->particlesCount(); i < n; i++ ) {
		m_particles.push_back( m_emitter->particles( i )->createInstance( materialFactory ) );
	}

	// Copy particle bursts
	for( s32 i = 0; i < emitter->burstCount(); i++ ) {
		m_bursts.push_back( emitter->burst( i ) );
	}
}

// ** EmitterInstance::isStopped
bool EmitterInstance::isStopped( void ) const
{
	return m_isStopped;
}

// ** EmitterInstance::setStopped
void EmitterInstance::setStopped( bool value )
{
	m_isStopped = value;
}

// ** EmitterInstance::calculateEmissionCount
s32 EmitterInstance::calculateEmissionCount( f32 scalar, s32 maxCount )
{
	// Resulting count
	s32 count = 0;

    // Calculate emission rate
    f32 rate = m_emitter->emission().sample( 0, scalar, -1.0f );

    if( rate > 0.0f ) {
        count = min2( maxCount, ( s32 )floor( m_timeEmission * rate ) );
        m_timeEmission -= count / rate;

        // Emission time can't be less than zero
		m_timeEmission = max2( m_timeEmission, 0.0f );
    }
	else if( rate == 0.0f ) {
		count			= 0;
		m_timeEmission	= 0.0f;
	}

	//  Emit particles by bursts
	for( u32 i = 0, n = ( u32 )m_bursts.size(); i < n; i++ ) {
		if( m_bursts[i].lastIteration == m_iteration ) {
			continue;
		}

		if( m_bursts[i].time <= m_time ) {
			count += m_bursts[i].count;
			m_bursts[i].lastIteration = m_iteration;
		}
	}

	count = min2( maxCount, count );

	return count;
}

// ** EmitterInstance::aliveCount
s32 EmitterInstance::aliveCount( void ) const
{
	return m_aliveCount;
}

// ** EmitterInstance::particlesCount
s32 EmitterInstance::particlesCount( void ) const
{
	return static_cast<s32>( m_particles.size() );
}

// ** EmitterInstance::particles
ParticlesInstanceWPtr EmitterInstance::particles( s32 index ) const
{
	DC_BREAK_IF( index < 0 || index >= particlesCount() );
	return m_particles[index];
}

// ** EmitterInstance::hasEnded
bool EmitterInstance::hasEnded( void ) const
{
	if( isLooped() ) {
		return false;
	}

	if( m_time < m_emitter->duration() ) {
		return false;
	}

	return m_aliveCount == 0;
}

// ** EmitterInstance::update
s32 EmitterInstance::update( f32 dt, const Vec3& position )
{
    f32		    duration = m_emitter->duration();
    ZoneWPtr    zone	 = m_emitter->zone();
    const Vec3& pos		 = m_emitter->position();

	// Update the emitter time
	bool ended = updateTime( dt );

	m_aliveCount = 0;

	// Update all particles instance inside this emitter
	for( u32 i = 0, n = ( u32 )m_particles.size(); i < n; i++ ) {
		// Store item at index
		ParticlesInstancePtr& particles = m_particles[i];

		// Update item and store alive count
		s32 count = particles->update( dt );

		// We have dead particles - ready to emit
		s32 deadCount = particles->maxCount() - count;

		if( deadCount && !ended && !m_isStopped ) {
			s32 emissionCount = calculateEmissionCount( m_time / duration, deadCount );

			if( emissionCount ) {
				m_emitter->emit( &particles->items(), count, count + emissionCount, position + pos, m_time / duration );
				particles->addAliveCount( emissionCount );
				particles->m_particles->update( &particles->items(), count, count + emissionCount, dt );
				count += emissionCount;
			}
		}

		// Increase the total alive count.
		m_aliveCount += count;
	}

	return m_aliveCount;
}

// ** EmitterInstance::updateTime
bool EmitterInstance::updateTime( f32 dt )
{
	f32 duration = m_emitter->duration();

	m_time += dt;
	m_timeEmission += dt;

	if( m_time < duration ) {
		return false;
	}

	if( isLooped() ) {
		m_iteration++;
		m_time -= floor( m_time / duration ) * duration;
	} else {
		m_time = duration;
	}

	return true;
}

// ** EmitterInstance::isLooped
bool EmitterInstance::isLooped( void ) const
{
    return m_emitter->isLooped() && !m_isStopped;
}

// ** EmitterInstance::warmUp
void EmitterInstance::warmUp( f32 dt, const Vec3& position )
{
	DC_BREAK_IF( m_emitter->isLooped() );

    s32 iterations = static_cast<s32>( m_emitter->duration() / dt );

    for( s32 i = 0; i < iterations; i++ ) {
        update( dt, position );
    }
}

} // namespace Fx

DC_END_DREEMCHEST