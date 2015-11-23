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
	Particles *particle = DC_NEW Particles;
	m_particles.push_back( particle );

	return particle;
}

// ** Emitter::createInstance
EmitterInstancePtr Emitter::createInstance( void ) const
{
	return DC_NEW EmitterInstance( const_cast<Emitter*>( this ) );
}

// ------------------------------------- EmitterInstance ---------------------------------------- //

// ** EmitterInstance::EmitterInstance
EmitterInstance::EmitterInstance( EmitterWPtr emitter ) : m_emitter( emitter ), m_time( 0.0f ), m_aliveCount( 0 )
{
	for( int i = 0, n = m_emitter->particlesCount(); i < n; i++ ) {
		m_particles.push_back( m_emitter->particles( i )->createInstance() );
	}
}

// ** EmitterInstance::aliveCount
s32 EmitterInstance::aliveCount( void ) const
{
	return m_aliveCount;
}

// ** EmitterInstance::hasEnded
bool EmitterInstance::hasEnded( void ) const
{
	if( m_emitter->isLooped() ) {
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

	for( u32 i = 0, n = ( u32 )m_particles.size(); i < n; i++ ) {
		m_aliveCount += m_particles[i]->update( zone, dt, position + pos, m_time / duration, ended );
	}

	return m_aliveCount;
}

// ** EmitterInstance::updateTime
bool EmitterInstance::updateTime( f32 dt )
{
	f32 duration = m_emitter->duration();

	m_time += dt;

	if( m_time < duration ) {
		return false;
	}

	if( m_emitter->isLooped() ) {
		m_time -= floor( m_time / duration ) * duration;
	} else {
		m_time = duration;
	}

	return true;
}

// ** EmitterInstance::warmUp
void EmitterInstance::warmUp( f32 dt, const Vec3& position )
{
    s32 iterations = static_cast<s32>( m_emitter->duration() / dt );

    for( s32 i = 0; i < iterations; i++ ) {
        update( dt, position );
    }
}

} // namespace Fx

DC_END_DREEMCHEST