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
Particles::Particles( void ) : m_count( 1 ), m_maxSnapshots( Particle::MaxSnapshots )
{
	setBlendingMode( BlendAlpha );
	setRenderingMode( RenderQuads );

    for( s32 i = 0; i < TotalColorParameters; i++ ) {
        m_color[i].setType( Parameter::Color );
    }

	// ** Set default values for parameters
	m_scalar[Life].setRange( 0.5f, 1.0f );
	m_scalar[Emission].setConstant( 1.0f );
	m_scalar[Direction].setRange( 0.0f, 360.0f );
	m_scalar[Rotation].setRange( -180.0f, 180.0f );
	m_scalar[Velocity].setRange( 25.0f, 50.0f );
	m_scalar[Size].setConstant( 10.0f );

	m_scalar[SizeOverLife].setConstant( 1.0f );
	m_scalar[TransparencyOverLife].setConstant( 1.0f );
	m_scalar[VelocityOverLife].setConstant( 1.0f );
	m_scalar[AngularVelocityOverLife].setConstant( 1.0f );
	m_scalar[TorqueOverLife].setConstant( 1.0f );
};

// ** Particles::createInstance
ParticlesInstancePtr Particles::createInstance( void ) const
{
	return ParticlesInstancePtr( DC_NEW ParticlesInstance( const_cast<Particles*>( this ) ) );
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
    const Parameter *emission = ScalarParam( Emission );
    return SampleParameter( emission, -1.0f );
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

    const Parameter* color           = ColorParam( Color );
	const Parameter* velocity        = ScalarParam( Velocity );
	const Parameter* angularVelocity = ScalarParam( AngularVelocity );
	const Parameter* torque          = ScalarParam( Torque );
	const Parameter* size            = ScalarParam( Size );
	const Parameter* life            = ScalarParam( Life );
	const Parameter* transparency    = ScalarParam( Transparency );
	const Parameter* direction       = ScalarParam( Direction );
	const Parameter* rotation        = ScalarParam( Rotation );

	Rgb white( 1.0f, 1.0f, 1.0f );
	int snapshotCount = snapshotsToSave();

	for( s32 i = 0; i < count; i++ ) {
		Particle& particle = particles[i];

		particle.m_linear.velocity      = SampleParameter( velocity, 0.0f );
		particle.m_angular.velocity     = SampleParameter( angularVelocity, 0.0f );
		particle.m_angular.torque       = SampleParameter( torque, 0.0f );
		particle.m_size.initial         = SampleParameter( size, 5.0f );
		particle.m_life.fade            = 1.0f / SampleParameter( life, 1.0f );
        particle.m_color.current.Rgb    = Rgb( 1.0f, 1.0f, 1.0f );
        particle.m_color.current.alpha  = 1.0f;
		particle.m_color.initial.alpha  = SampleKoeficient( transparency, 1 );
        particle.m_color.initial.Rgb    = SampleParameter( color, white );
		particle.m_direction            = SampleParameter( direction, 0.0f );
		particle.m_rotation             = SampleParameter( rotation, 0.0f );
		particle.m_force.velocity       = Vec2( 0, 0 );
		particle.m_life.age             = 0.0f;

		Vec3 pos = zone.valid() ? zone->generateRandomPoint( scalar, position ) : position;
        particle.m_position = Vec2( pos.x, pos.y );

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
		particle.m_snapshots[0].color = particle.m_color.current.Rgb;
		particle.m_snapshots[0].alpha = particle.m_color.current.alpha;
		particle.m_snapshots[0].size  = particle.m_size.current;
	}
}

// ** Particles::initSnapshots
void Particles::initSnapshots( Particle& particle, s32 count ) const
{
	for( s32 i = 0; i < count; i++ ) {
		particle.m_snapshots[i].pos   = particle.m_position;
		particle.m_snapshots[i].color = particle.m_color.current.Rgb;
		particle.m_snapshots[i].alpha = particle.m_color.current.alpha;
		particle.m_snapshots[i].size  = particle.m_size.current;
	}
}

// ** Particles::update
s32 Particles::update( Particle* particles, s32 count, f32 dt, Bounds* bounds ) const
{
	const Parameter *color           = ColorParam( ColorOverLife );
	const Parameter *alpha           = ScalarParam( TransparencyOverLife );
	const Parameter *velocity        = ScalarParam( VelocityOverLife );
	const Parameter *torque          = ScalarParam( TorqueOverLife );
	const Parameter *angularVelocity = ScalarParam( AngularVelocityOverLife );
	const Parameter *size            = ScalarParam( SizeOverLife );

	int aliveCount = 0;
	Rgb white( 1.0f, 1.0f, 1.0f );

	// ** Reset in-out parameters
	if( bounds ) *bounds = Bounds();

	// ** Particles
	for( s32 i = 0; i < count; i++ ) {
		Particle& particle = particles[i];
        f32       scalar   = 0.0f;
        
        particle.m_life.age += particle.m_life.fade * dt;
		scalar               = particle.m_life.age;

		if( scalar > 1.0f ) {
            count        = count - 1;
            particles[i] = particles[count];
            i            = i - 1;
			continue;
		}

		particle.m_direction            += particle.m_angular.velocity    * SampleKoeficient( angularVelocity,  1.0f );
		particle.m_position             += particle.m_linear.velocity     * SampleKoeficient( velocity,         1.0f ) * Vec2::fromAngle( particle.m_direction ) * dt + particle.m_force.velocity * dt;
		particle.m_rotation             += particle.m_angular.torque      * SampleKoeficient( torque,           1.0f ) * dt;
        particle.m_color.current.Rgb     = particle.m_color.initial.Rgb   * SampleParameter( color, white );
		particle.m_color.current.alpha   = particle.m_color.initial.alpha * SampleKoeficient( alpha,            1.0f );
		particle.m_size.current          = particle.m_size.initial        * SampleKoeficient( size,             1.0f );

		if( m_blendingMode != BlendAlpha ) {
			f32 alpha = particle.m_color.initial.alpha;
			particle.m_color.current.Rgb = particle.m_color.initial.Rgb * particle.m_color.current.alpha;
			particle.m_color.initial.alpha = alpha;
		}

		// ** Update bounds
		if( bounds ) {
			*bounds += Vec3( particle.m_position.x, particle.m_position.y, 0.0f );
		}

		// ** Increase alive counter
        aliveCount++;
	}

	return aliveCount;
}

// ------------------------------------------------- ParticlesInstance ------------------------------------------------- //

// ** ParticlesInstance::ParticlesInstance
ParticlesInstance::ParticlesInstance( ParticlesWPtr particles ) : m_particles( particles ), m_aliveCount( 0 ), m_time( 0.0f ), m_emissionTime( 0.0f ), m_snapshotTime( 0.0f )
{
	m_items.resize( m_particles->count() );

    for( u32 i = 0, n = m_particles->count(); i < n; i++ ) {
        m_items[i].m_life.age  = 2.0f;
        m_items[i].m_life.fade = 0.0f;
    }
}

// ** ParticlesInstance::aliveCount
int ParticlesInstance::aliveCount( void ) const
{
	return m_aliveCount;
}

// ** ParticlesInstance::update
s32 ParticlesInstance::update( const ZoneWPtr& zone, f32 dt, const Vec3& position, f32 scalar, bool noEmission )
{
	m_time          += dt;
    m_emissionTime  += dt;

    // ** Update particles
	m_aliveCount  = m_particles->update( &m_items[0], m_aliveCount, dt, &m_bounds );

	// ** Save particle snapshots for paths
	if( (m_time - m_snapshotTime) >= 0.01f ) {
		m_particles->savePaths( &m_items[0], m_aliveCount );
		m_snapshotTime = m_time;
	}

    // ** Calculate dead count
    s32 deadCount = ( s32 )m_items.size() - m_aliveCount;

	if( deadCount == 0 || noEmission ) {
        return m_aliveCount;
    }

    // ** Calculate emission amount
    f32 emission = m_particles->emission( scalar );

    if( emission > 0.0f ) {
        deadCount       = std::min( deadCount, ( s32 )floor( m_emissionTime * emission ) );
        m_emissionTime -= deadCount / emission;

        DC_BREAK_IF( m_emissionTime < 0.0f );
    }
	else if( emission == 0.0f ) {
		deadCount		= 0;
		m_emissionTime	= 0.0f;
	}

    m_aliveCount += m_particles->init( zone, &m_items[0] + m_aliveCount, position, deadCount, scalar );

	return m_aliveCount;
}

} // namespace Fx

DC_END_DREEMCHEST
