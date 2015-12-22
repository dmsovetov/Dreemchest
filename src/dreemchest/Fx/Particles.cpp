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

#include "Particles.h"

#include "Renderers.h"
#include "Zones.h"

#define ScalarParam( name ) m_scalar[name] ? &m_scalar[name] : NULL
#define ColorParam( name )  m_color[name]  ? &m_color[name]  : NULL

DC_BEGIN_DREEMCHEST

namespace Fx {

// ----------------------------------------------- Particles ----------------------------------------------- //

// ** Particles::Particles
Particles::Particles( EmitterWPtr emitter ) : m_emitter( emitter ), m_count( 1 ), m_maxSnapshots( Particle::MaxSnapshots )
{
	setBlendingMode( BlendAlpha );
	setRenderingMode( RenderQuads );
};

// ** Particles::createInstance
ParticlesInstancePtr Particles::createInstance( IMaterialFactoryWPtr materialFactory ) const
{
	return ParticlesInstancePtr( DC_NEW ParticlesInstance( materialFactory, const_cast<Particles*>( this ) ) );
}

// ** Particles::emitter
EmitterWPtr Particles::emitter( void ) const
{
	return m_emitter;
}

// ** Particles::count
s32 Particles::count( void ) const
{
	return m_count;
}

// ** Particles::setCount
void Particles::setCount( s32 value )
{
	m_count = value;
}

// ** Particles::name
const String& Particles::name( void ) const
{
	return m_name;
}

// ** Particles::setName
void Particles::setName( const String& value )
{
	m_name = value;
}

// ** Particles::burstCount
s32 Particles::burstCount( void ) const
{
	return ( s32 )m_bursts.size();
}

// ** Particles::burst
const ParticleBurst& Particles::burst( s32 index ) const
{
	DC_BREAK_IF( index < 0 || index >= burstCount() );
	return m_bursts[index];
}

// ** Particles::addBurst
void Particles::addBurst( f32 time, s32 count )
{
	ParticleBurst burst;
	burst.time			= time;
	burst.count			= count;
	burst.lastIteration	= -1;
	m_bursts.push_back( burst );
}

// ** Particles::material
const String& Particles::material( void ) const
{
	return m_material;
}

// ** Particles::setMaterial
void Particles::setMaterial( const String& value )
{
	m_material = value;
}

// ** Particles::blendingMode
BlendingMode Particles::blendingMode( void ) const
{
	return m_blendingMode;
}

// ** Particles::setBlendingMode
void Particles::setBlendingMode( BlendingMode value )
{
	m_blendingMode = value;
}

// ** Particles::renderingMode
RenderingMode Particles::renderingMode( void ) const
{
	return m_renderingMode;
}

// ** Particles::setRenderingMode
void Particles::setRenderingMode( RenderingMode value )
{
	m_renderingMode = value;
}

// ** Particles::emission
f32 Particles::emission( f32 scalar ) const
{
    const FloatParameter *emission = ScalarParam( Emission );
    return SampleParameter( 0, emission, -1.0f );
}

// ** Particles::maxSnapshots
s32 Particles::maxSnapshots( void ) const
{
	return m_maxSnapshots;
}

// ** Particles::setMaxSnapshots
void Particles::setMaxSnapshots( s32 value )
{
	m_maxSnapshots = max2( 4, value );
}

// ** Particles::scalarParameter
const FloatParameter& Particles::scalarParameter( ScalarParameter parameter ) const
{
	return m_scalar[parameter];
}

// ** Particles::scalarParameter
FloatParameter& Particles::scalarParameter( ScalarParameter parameter )
{
	return m_scalar[parameter];
}

// ** Particles::colorParameter
const RgbParameter& Particles::colorParameter( ColorParameter parameter ) const
{
	return m_color[parameter];
}

// ** Particles::colorParameter
RgbParameter& Particles::colorParameter( ColorParameter parameter )
{
	return m_color[parameter];
}

// ** Particles::snapshotsToSave
s32 Particles::snapshotsToSave( void ) const
{
	if( (m_renderingMode == RenderPaths) || (m_renderingMode == RenderThickPaths) ) {
		return m_maxSnapshots;
	}

	return 0;
}

// ** Particles::init
s32 Particles::init( const ZoneWPtr& zone, Particle* particles, const Vec3& position, s32 count, f32 scalar ) const
{
    if( !count ) {
        return 0;
    }

    const RgbParameter*   color           = ColorParam( Color );
	const FloatParameter* accelerationX   = ScalarParam( AccelerationX );
	const FloatParameter* accelerationY   = ScalarParam( AccelerationY );
	const FloatParameter* accelerationZ   = ScalarParam( AccelerationZ );
	const FloatParameter* angularVelocity = ScalarParam( AngularVelocity );
	const FloatParameter* speed           = ScalarParam( Speed );
	const FloatParameter* torque          = ScalarParam( Torque );
	const FloatParameter* size            = ScalarParam( Size );
	const FloatParameter* life            = ScalarParam( Life );
	const FloatParameter* transparency    = ScalarParam( Transparency );
	const FloatParameter* direction       = ScalarParam( Direction );
	const FloatParameter* rotation        = ScalarParam( Rotation );
	const FloatParameter* gravity         = ScalarParam( Gravity );

	Rgb white( 1.0f, 1.0f, 1.0f );
	s32 snapshotCount = snapshotsToSave();

	for( s32 i = 0; i < count; i++ ) {
		Particle& particle = particles[i];

		particle.m_index				= rand() % FloatParameter::LifetimeRandomizationCount;
		particle.m_force.acceleration	= Vec3( SampleParameter( particle.m_index, accelerationX, 0.0f ), SampleParameter( particle.m_index, accelerationY, 0.0f ) - SampleParameter( particle.m_index, gravity, 0.0f ), SampleParameter( particle.m_index, accelerationZ, 0.0f ) );
		particle.m_angular.velocity     = SampleParameter( particle.m_index, angularVelocity, 0.0f );
		particle.m_angular.torque       = SampleParameter( particle.m_index, torque, 0.0f );
		particle.m_size.initial         = SampleParameter( particle.m_index, size, 5.0f );
		particle.m_life.initial			= SampleParameter( particle.m_index, life, 1.0f );
		particle.m_life.current			= particle.m_life.initial;
        particle.m_color.current.rgb    = Rgb( 1.0f, 1.0f, 1.0f );
        particle.m_color.current.alpha  = 1.0f;
		particle.m_color.initial.alpha  = SampleParameter( particle.m_index, transparency, 1 );
        particle.m_color.initial.rgb    = SampleParameter( particle.m_index, color, white );
		particle.m_direction            = SampleParameter( particle.m_index, direction, 0.0f );
		particle.m_rotation             = SampleParameter( particle.m_index, rotation, 0.0f );

		Zone::Point point		  = zone.valid() ? zone->generateRandomPoint( scalar, position ) : Zone::Point( position, Vec3( 0.0f, 0.0f, 0.0f ) );
        particle.m_position		  = point.position;
		particle.m_force.velocity = point.direction * SampleParameter( particle.m_index, speed, 0.0f );

		// ** Init snapshots
		if( snapshotCount ) {
			initSnapshots( particle, snapshotCount );
		}

		update( &particle, 1, 0.0f );
	}

    return count;
}

// ** Particles::savePaths
void Particles::savePaths( Particle* items, s32 itemCount ) const
{
	s32 snapshotCount = snapshotsToSave();

	for( s32 i = 0; i < itemCount; i++ ) {
		Particle& particle = items[i];

		for( int j = snapshotCount - 2; j >= 1; j-- ) {
			particle.m_snapshots[j].pos   = particle.m_snapshots[j - 1].pos;
			particle.m_snapshots[j].color = particle.m_snapshots[j - 1].color;
			particle.m_snapshots[j].alpha = particle.m_snapshots[j - 1].alpha;
			particle.m_snapshots[j].size  = particle.m_snapshots[j - 1].size;
		}

		particle.m_snapshots[0].pos   = particle.m_position;
		particle.m_snapshots[0].color = particle.m_color.current.rgb;
		particle.m_snapshots[0].alpha = particle.m_color.current.alpha;
		particle.m_snapshots[0].size  = particle.m_size.current;
	}
}

// ** Particles::initSnapshots
void Particles::initSnapshots( Particle& particle, s32 count ) const
{
	for( s32 i = 0; i < count; i++ ) {
		particle.m_snapshots[i].pos   = particle.m_position;
		particle.m_snapshots[i].color = particle.m_color.current.rgb;
		particle.m_snapshots[i].alpha = particle.m_color.current.alpha;
		particle.m_snapshots[i].size  = particle.m_size.current;
	}
}

// ** Particles::update
s32 Particles::update( Particle* particles, s32 count, f32 dt, Bounds* bounds ) const
{
	const RgbParameter*   color           = ColorParam( ColorOverLife );
	const FloatParameter* alpha           = ScalarParam( TransparencyOverLife );
	const FloatParameter* velocityX       = ScalarParam( VelocityXOverLife );
	const FloatParameter* velocityY       = ScalarParam( VelocityYOverLife );
	const FloatParameter* velocityZ       = ScalarParam( VelocityZOverLife );
	const FloatParameter* accelerationX   = ScalarParam( AccelerationXOverLife );
	const FloatParameter* accelerationY   = ScalarParam( AccelerationYOverLife );
	const FloatParameter* accelerationZ   = ScalarParam( AccelerationZOverLife );
	const FloatParameter* torque          = ScalarParam( TorqueOverLife );
	const FloatParameter* angularVelocity = ScalarParam( AngularVelocityOverLife );
	const FloatParameter* size            = ScalarParam( SizeOverLife );

	s32  aliveCount = 0;
	Rgb  white( 1.0f, 1.0f, 1.0f );
	Vec3 zero( 0.0f, 0.0f, 0.0f );

	// ** Reset in-out parameters
	if( bounds ) *bounds = Bounds();

	// ** Particles
	for( s32 i = 0; i < count; i++ ) {
		Particle& particle = particles[i];
        f32       scalar   = 0.0f;

		particle.m_life.current -= dt;
		scalar = 1.0f - particle.m_life.current / particle.m_life.initial;

		if( particle.m_life.current < 0.0f ) {
            count        = count - 1;
            particles[i] = particles[count];
            i            = i - 1;
			continue;
		}
		
		Vec3 velocity = Vec3( SampleParameter( particle.m_index, velocityX, 0.0f ), SampleParameter( particle.m_index, velocityY, 0.0f ), SampleParameter( particle.m_index, velocityZ, 0.0f ) );

		particle.m_direction            += particle.m_angular.velocity    * SampleKoeficient( particle.m_index, angularVelocity,  1.0f );
		particle.m_force.velocity		+= particle.m_force.acceleration * dt;
		particle.m_position			    += (velocity + particle.m_force.velocity) * dt;
		particle.m_rotation				+= particle.m_angular.velocity * dt;
        particle.m_color.current.rgb     = particle.m_color.initial.rgb   * SampleParameter( particle.m_index, color, white );
		particle.m_color.current.alpha   = particle.m_color.initial.alpha * SampleParameter( particle.m_index, alpha,            1.0f );
		particle.m_size.current          = particle.m_size.initial        * SampleParameter( particle.m_index, size,             1.0f );

		if( m_blendingMode != BlendAlpha ) {
			f32 alpha = particle.m_color.initial.alpha;
			particle.m_color.current.rgb = particle.m_color.initial.rgb * particle.m_color.current.alpha;
			particle.m_color.initial.alpha = alpha;
		}

		// ** Update bounds
		if( bounds ) {
			*bounds << Vec3( particle.m_position.x - particle.m_size.current * 0.5f, particle.m_position.y - particle.m_size.current * 0.5f, 0.0f );
			*bounds << Vec3( particle.m_position.x + particle.m_size.current * 0.5f, particle.m_position.y + particle.m_size.current * 0.5f, 0.0f );
		}

		// ** Increase alive counter
        aliveCount++;
	}

	return aliveCount;
}

// ------------------------------------------------- ParticlesInstance ------------------------------------------------- //

// ** ParticlesInstance::ParticlesInstance
ParticlesInstance::ParticlesInstance( IMaterialFactoryWPtr materialFactory, ParticlesWPtr particles ) : m_particles( particles ), m_aliveCount( 0 ), m_time( 0.0f ), m_emissionTime( 0.0f ), m_snapshotTime( 0.0f )
{
	m_items.resize( m_particles->count() );

	for( s32 i = 0; i < particles->burstCount(); i++ ) {
		m_bursts.push_back( particles->burst( i ) );
	}

	if( materialFactory.valid() ) {
		m_material = materialFactory->createMaterial( particles->material() );
	}
}

// ** ParticlesInstance::aliveCount
s32 ParticlesInstance::aliveCount( void ) const
{
	return m_aliveCount;
}

// ** ParticlesInstance::items
const Particle* ParticlesInstance::items( void ) const
{
	return &m_items[0];
}

// ** ParticlesInstance::blendingMode
BlendingMode ParticlesInstance::blendingMode( void ) const
{
	return m_particles->blendingMode();
}

// ** ParticlesInstance::renderingMode
RenderingMode ParticlesInstance::renderingMode( void ) const
{
	return m_particles->renderingMode();
}

// ** ParticlesInstance::material
IMaterialWPtr ParticlesInstance::material( void ) const
{
	return m_material;
}

// ** ParticlesInstance::setMaterial
void ParticlesInstance::setMaterial( const IMaterialWPtr& value )
{
	m_material = value;
}

// ** ParticlesInstance::bounds
const Bounds& ParticlesInstance::bounds( void ) const
{
	return m_bounds;
}

// ** ParticlesInstance::update
s32 ParticlesInstance::update( s32 iteration, const ZoneWPtr& zone, f32 dt, const Vec3& position, f32 scalar, bool noEmission )
{
	m_time          += dt;
    m_emissionTime  += dt;

    // Update particles
	m_aliveCount  = m_particles->update( &m_items[0], m_aliveCount, dt, &m_bounds );

	// Save particle snapshots for paths
	if( (m_time - m_snapshotTime) >= 0.01f ) {
		m_particles->savePaths( &m_items[0], m_aliveCount );
		m_snapshotTime = m_time;
	}

    // Calculate dead count
    s32 deadCount = ( s32 )m_items.size() - m_aliveCount;

	if( deadCount == 0 || noEmission ) {
        return m_aliveCount;
    }

    // Calculate emission amount
    f32 emission = m_particles->emission( scalar );

    if( emission > 0.0f ) {
        deadCount       = min2( deadCount, ( s32 )floor( m_emissionTime * emission ) );
        m_emissionTime -= deadCount / emission;

        DC_BREAK_IF( m_emissionTime < 0.0f );
    }
	else if( emission == 0.0f ) {
		deadCount		= 0;
		m_emissionTime	= 0.0f;
	}

	//  Emit particles by bursts
	for( u32 i = 0, n = ( u32 )m_bursts.size(); i < n; i++ ) {
		if( m_bursts[i].lastIteration == iteration ) {
			continue;
		}

		if( m_bursts[i].time <= m_time ) {
			deadCount += m_bursts[i].count;
			m_bursts[i].lastIteration = iteration;
		}
	}

	if( deadCount == 0 ) {
		return m_aliveCount;
	}

	// Clamp dead particles count
	deadCount = max2( 1, min2( deadCount, ( s32 )m_items.size() - 1 ) );

    m_aliveCount += m_particles->init( zone, &m_items[0] + m_aliveCount, position, deadCount, scalar );

	return m_aliveCount;
}

} // namespace Fx

DC_END_DREEMCHEST
