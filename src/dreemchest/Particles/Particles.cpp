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

#include	"Particles.h"

#include	"Renderers.h"
#include    "zone/Zones.h"

//#include	"../asset/TextureAsset.h"

#define     ScalarParam( name ) m_scalar[name] ? &m_scalar[name] : NULL
#define     ColorParam( name )  m_color[name]  ? &m_color[name]  : NULL

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

DC_BEGIN_DREEMCHEST

namespace Particles {

// ----------------------------------------------- ParticleModel ----------------------------------------------- //

// ** ParticleModel::ParticleModel
ParticleModel::ParticleModel( void )
{
	m_count				= 1;
	m_particleRenderer  = NULL;
	m_texture			= NULL;
	m_maxSnapshots		= sParticle::MaxSnapshots;

	setBlendMode( BlendNormal );
	setRendererType( RenderQuads );

	// ** Register parameters
	registerParameter( "Emission", &m_scalar[Emission], sParameterInfo::Initial );
	registerParameter( "Life", &m_scalar[Life], sParameterInfo::Initial );
	registerParameter( "Direction", &m_scalar[Direction], sParameterInfo::Initial );
	registerParameter( "Size", &m_scalar[Size], sParameterInfo::Initial );
	registerParameter( "Transparency", &m_scalar[Transparency], sParameterInfo::Initial );
	registerParameter( "Velocity", &m_scalar[Velocity], sParameterInfo::Initial );
	registerParameter( "AngularVelocity", &m_scalar[AngularVelocity], sParameterInfo::Initial );
	registerParameter( "Torque", &m_scalar[Torque], sParameterInfo::Initial );
	registerParameter( "Rotation", &m_scalar[Rotation], sParameterInfo::Initial );
	registerParameter( "SizeOverLife", &m_scalar[SizeOverLife], sParameterInfo::Lifetime );
	registerParameter( "TransparencyOverLife", &m_scalar[TransparencyOverLife], sParameterInfo::Lifetime );
	registerParameter( "VelocityOverLife", &m_scalar[VelocityOverLife], sParameterInfo::Lifetime );
	registerParameter( "AngularVelocityOverLife", &m_scalar[AngularVelocityOverLife], sParameterInfo::Lifetime );
	registerParameter( "TorqueOverLife", &m_scalar[TorqueOverLife], sParameterInfo::Lifetime );

    registerParameter( "Color", &m_color[Color], sParameterInfo::Initial );
    registerParameter( "ColorOverLife", &m_color[ColorOverLife], sParameterInfo::Lifetime );

    for( int i = 0; i < TotalColorParameters; i++ ) {
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

ParticleModel::~ParticleModel( void )
{
	DC_NOT_IMPLEMENTED;
//	DC_RELEASE( m_texture );
//	delete m_particleRenderer;
}

// ** ParticleModel::createInstance
Particles* ParticleModel::createInstance( void ) const
{
	return DC_NEW Particles( this );
}

// ** ParticleModel::count
int ParticleModel::count( void ) const
{
	return m_count;
}

// ** ParticleModel::setCount
void ParticleModel::setCount( int value )
{
	m_count = value;
}

// ** ParticleModel::name
const char* ParticleModel::name( void ) const
{
	return m_name.c_str();
}

// ** ParticleModel::setName
void ParticleModel::setName( const char *value )
{
	m_name = value;
}

// ** ParticleModel::texture
dcTextureAsset ParticleModel::texture( void ) const
{
	return m_texture;
}

// ** ParticleModel::setTexture
void ParticleModel::setTexture( dcTextureAsset value )
{
	DC_NOT_IMPLEMENTED;
//	DC_RELEASE( m_texture );
//
//	if( (m_texture = value) ) {
//		m_texture->Ref();
//	}
}

// ** ParticleModel::rendererType
RendererType ParticleModel::rendererType( void ) const
{
	return m_particleRenderer ? m_particleRenderer->type() : TotalRenderTypes;
}

// ** ParticleModel::blendMode
BlendMode ParticleModel::blendMode( void ) const
{
	return m_blendMode;
}

// ** ParticleModel::setBlendMode
void ParticleModel::setBlendMode( BlendMode value )
{
	m_blendMode = value;
}

// ** ParticleModel::setRendererType
void ParticleModel::setRendererType( RendererType value )
{
	if( m_particleRenderer && m_particleRenderer->type() == value ) {
		return;
	}

	delete m_particleRenderer;
	m_particleRenderer = Renderer::createRenderer( value );
}

// ** ParticleModel::renderer
Renderer* ParticleModel::renderer( void ) const
{
	return m_particleRenderer;
}

// ** ParticleModel::emission
float ParticleModel::emission( float scalar ) const
{
    const Parameter *emission = ScalarParam( Emission );
    return SampleParameter( emission, -1.0f );
}

// ** ParticleModel::maxSnapshots
int ParticleModel::maxSnapshots( void ) const
{
	return m_maxSnapshots;
}

// ** ParticleModel::setMaxSnapshots
void ParticleModel::setMaxSnapshots( int value )
{
	m_maxSnapshots = std::max( 4, value );
}

// ** ParticleModel::snapshotsToSave
int ParticleModel::snapshotsToSave( void ) const
{
	RendererType rendererType = m_particleRenderer->type();

	if( (rendererType == RenderPaths) || (rendererType == RenderThickPaths) ) {
		return m_maxSnapshots;
	}

	return 0;
}

// ** ParticleModel::render
void ParticleModel::render( dcBatchRenderer renderer, const sParticle *particles, int count ) const
{
	m_particleRenderer->render( renderer, m_texture, m_blendMode, particles, count );
}

// ** ParticleModel::init
int ParticleModel::init( Zone *zone, sParticle *particles, const Vec2& position, int count, float scalar ) const
{
    if( !count ) {
        return 0;
    }

    const Parameter *color           = ColorParam( Color );
	const Parameter *velocity        = ScalarParam( Velocity );
	const Parameter *angularVelocity = ScalarParam( AngularVelocity );
	const Parameter *torque          = ScalarParam( Torque );
	const Parameter *size            = ScalarParam( Size );
	const Parameter *life            = ScalarParam( Life );
	const Parameter *transparency    = ScalarParam( Transparency );
	const Parameter *direction       = ScalarParam( Direction );
	const Parameter *rotation        = ScalarParam( Rotation );

	Rgb white( 1.0f, 1.0f, 1.0f );
	int snapshotCount = snapshotsToSave();

	for( int i = 0; i < count; i++ ) {
		sParticle& particle = particles[i];

		particle.m_linear.velocity      = SampleParameter( velocity, 0.0f );
		particle.m_angular.velocity     = SampleParameter( angularVelocity, 0.0f );
		particle.m_angular.torque       = SampleParameter( torque, 0.0f );
		particle.m_size.initial         = SampleParameter( size, 5.0f );
		particle.m_life.fade            = 1.0f / SampleParameter( life, 1.0f );
        particle.m_color.current.Rgb    = white;
        particle.m_color.current.alpha  = 1.0f;
		particle.m_color.initial.alpha  = SampleKoeficient( transparency, 1 );
        particle.m_color.initial.Rgb    = SampleParameter( color, white );
		particle.m_direction            = SampleParameter( direction, 0.0f );
		particle.m_rotation             = SampleParameter( rotation, 0.0f );
		particle.m_force.velocity       = Vec2( 0, 0 );
		particle.m_life.age             = 0.0f;
        particle.m_position             = zone ? zone->generateRandomPoint( scalar, position ) : position;

		// ** Init snapshots
		if( snapshotCount ) {
			initSnapshots( particle, snapshotCount );
		}

		update( &particle, 1, 0.0f );
	}

    return count;
}

// ** ParticleModel::savePaths
void ParticleModel::savePaths( sParticle* items, int itemCount ) const
{
	int snapshotCount = snapshotsToSave();

	for( int i = 0; i < itemCount; i++ ) {
		sParticle& particle = items[i];

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

// ** ParticleModel::initSnapshots
void ParticleModel::initSnapshots( sParticle& particle, int count ) const
{
	for( int i = 0; i < count; i++ ) {
		particle.m_snapshots[i].pos   = particle.m_position;
		particle.m_snapshots[i].color = particle.m_color.current.Rgb;
		particle.m_snapshots[i].alpha = particle.m_color.current.alpha;
		particle.m_snapshots[i].size  = particle.m_size.current;
	}
}

// ** ParticleModel::update
int ParticleModel::update( sParticle *particles, int count, float dt, Vec2 *min, Vec2 *max ) const
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
	if( max ) *max = Vec2( -FLT_MAX, -FLT_MAX );
	if( min ) *min = Vec2(  FLT_MAX,  FLT_MAX );

	// ** Particles
	for( int i = 0; i < count; i++ ) {
		sParticle& particle = particles[i];
        float      scalar   = 0.0f;
        
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

		if( m_blendMode != BlendNormal ) {
			float alpha = particle.m_color.initial.alpha;
			particle.m_color.current.Rgb = particle.m_color.initial.Rgb * particle.m_color.current.alpha;
			particle.m_color.initial.alpha = alpha;
		}

		// ** Update bounds
		if( min && max ) {
			float x = particle.m_position.x;
			float y = particle.m_position.y;

			if( x > max->x )			max->x = x;
			else if( x < min->x )	min->x = x;
			if( y > max->y )			max->y = y;
			else if( y < min->y )	min->y = y;
		}

		// ** Increase alive counter
        aliveCount++;
	}

	return aliveCount;
}

// ------------------------------------------------- Particles ------------------------------------------------- //

// ** Particles::Particles
Particles::Particles( const ParticleModel *model ) : m_model( model ), m_particles( NULL ), m_count( model->count() ), m_aliveCount( 0 ), m_time( 0.0f ), m_emissionTime( 0.0f ), m_snapshotTime( 0.0f )
{
	m_count		= m_model->count();
	m_particles = new sParticle[m_count];

    for( int i = 0; i < m_count; i++ ) {
        m_particles[i].m_life.age  = 2.0f;
        m_particles[i].m_life.fade = 0.0f;
    }
}

Particles::~Particles( void )
{
	delete[]m_particles;
}

// ** Particles::aliveCount
int Particles::aliveCount( void ) const
{
	return m_aliveCount;
}

// ** Particles::update
int Particles::update( Zone *zone, float dt, const Vec2& position, float scalar, bool noEmission )
{
	m_time          += dt;
    m_emissionTime  += dt;

    // ** Update particles
	m_aliveCount  = m_model->update( m_particles, m_aliveCount, dt, &m_min, &m_max );

	// ** Save particle snapshots for paths
	if( (m_time - m_snapshotTime) >= 0.01f ) {
		m_model->savePaths( m_particles, m_aliveCount );
		m_snapshotTime = m_time;
	}

    // ** Calculate dead count
    int deadCount = m_count - m_aliveCount;

	if( deadCount == 0 || noEmission ) {
        return m_aliveCount;
    }

    // ** Calculate emission amount
    float emission = m_model->emission( scalar );
    if( emission > 0.0f ) {
        deadCount       = std::min( deadCount, ( int )floor( m_emissionTime * emission ) );
        m_emissionTime -= deadCount / emission;

        DC_BREAK_IF( m_emissionTime < 0.0f );
    }
	else if( emission == 0.0f ) {
		deadCount		= 0;
		m_emissionTime	= 0.0f;
	}

    m_aliveCount += m_model->init( zone, m_particles + m_aliveCount, position, deadCount, scalar );

	return m_aliveCount;
}

// ** Particles::render
void Particles::render( dcBatchRenderer renderer )
{
    m_model->render( renderer, m_particles, m_aliveCount );
}

} // namespace Particles

DC_END_DREEMCHEST