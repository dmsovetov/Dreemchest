//
//  Filename:   Emitter.cpp
//	Created:	28:05:2011   18:32

/*
 =========================================================================================

            HEADERS & DEFS

 =========================================================================================
 */

#include    "Emitter.h"

#include	"Particles.h"

#define     EmitterParam( name ) m_parameter[name] ? &m_parameter[name] : NULL;

/*
 =========================================================================================

            CODE

 =========================================================================================
 */

namespace dreemchest {

namespace particles {

// ---------------------------------- EmitterModel ------------------------------------- //

// ** EmitterModel::EmitterModel
EmitterModel::EmitterModel( void ) : m_position( 0, 0 ), m_duration( 1.0f ), m_isLooped( true )
{
    registerZone( "Disk", ZoneType::Disk );
    registerZone( "Line", ZoneType::Line );
}

EmitterModel::~EmitterModel( void )
{
	for( int i = 0, n = totalParticles(); i < n; i++ ) {
		delete m_particles[i];
	}
}

// ** EmitterModel::position
const vec2& EmitterModel::position( void ) const
{
	return m_position;
}

// ** EmitterModel::setPosition
void EmitterModel::setPosition( const vec2& value )
{
	m_position = value;
}

// ** EmitterModel::duration
float EmitterModel::duration( void ) const
{
	return m_duration;
}

// ** EmitterModel::setDuration
void EmitterModel::setDuration( float value )
{
	m_duration = std::max( value, 0.1f );
}

// ** EmitterModel::name
const char* EmitterModel::name( void ) const
{
	return m_name.c_str();
}

// ** EmitterModel::setName
void EmitterModel::setName( const char *value )
{
	m_name = value;
}

// ** EmitterModel::isLooped
bool EmitterModel::isLooped( void ) const
{
	return m_isLooped;
}

// ** EmitterModel::setLooped
void EmitterModel::setLooped( bool value )
{
	m_isLooped = value;
}

// ** EmitterModel::totalParticles
int EmitterModel::totalParticles( void ) const
{
	return ( int )m_particles.size();
}

// ** EmitterModel::particles
ParticleModel* EmitterModel::particles( int index ) const
{
	DC_BREAK_IF( index < 0 || index >= totalParticles() );
	return m_particles[index];
}

// ** EmitterModel::removeParticles
void EmitterModel::removeParticles( ParticleModel *particle )
{
	ParticleModelArray::iterator i = std::find( m_particles.begin(), m_particles.end(), particle );
	if( i == m_particles.end() ) {
		DC_BREAK;
		return;
	}

	delete *i;
	m_particles.erase( i );
}

// ** EmitterModel::createParticles
ParticleModel* EmitterModel::createParticles( void )
{
	ParticleModel *particle = DC_NEW ParticleModel;
	m_particles.push_back( particle );

	return particle;
}

// ** EmitterModel::createInstance
Emitter* EmitterModel::createInstance( void ) const
{
	return DC_NEW Emitter( this );
}

// ------------------------------------- Emitter ---------------------------------------- //

// ** Emitter::Emitter
Emitter::Emitter( const EmitterModel *model ) : m_model( model ), m_time( 0.0f ), m_aliveCount( 0 )
{
	for( int i = 0, n = m_model->totalParticles(); i < n; i++ ) {
		m_particles.push_back( m_model->particles( i )->createInstance() );
	}
}

Emitter::~Emitter( void )
{
	for( int i = 0, n = ( int )m_particles.size(); i < n; i++ ) {
		delete m_particles[i];
	}
}

// ** Emitter::aliveCount
int Emitter::aliveCount( void ) const
{
	return m_aliveCount;
}

// ** Emitter::hasEnded
bool Emitter::hasEnded( void ) const
{
	if( m_model->isLooped() ) {
		return false;
	}

	if( m_time < m_model->duration() ) {
		return false;
	}

	return m_aliveCount == 0;
}

// ** Emitter::update
int Emitter::update( float dt, const vec2& position )
{
    const float duration = m_model->duration();
	bool		ended	 = updateTime( dt );

    Zone*       zone = m_model->zone();
    const vec2& pos  = m_model->position();

	m_aliveCount = 0;

	for( int i = 0, n = ( int )m_particles.size(); i < n; i++ ) {
		m_aliveCount += m_particles[i]->update( zone, dt, position + pos, m_time / duration, ended );
	}

	return m_aliveCount;
}

// ** Emitter::updateTime
bool Emitter::updateTime( float dt )
{
	float duration = m_model->duration();

	m_time += dt;

	if( m_time < duration ) {
		return false;
	}

	if( m_model->isLooped() ) {
		m_time -= floor( m_time / duration ) * duration;
	} else {
		m_time = duration;
	}

	return true;
}

// ** Emitter::warmUp
void Emitter::warmUp( float dt, const vec2& position )
{
    int iterations = m_model->duration() / dt;

    for( int i = 0; i < iterations; i++ ) {
        update( dt, position );
    }
}

// ** Emitter::render
void Emitter::render( dcBatchRenderer renderer )
{
	for( int i = 0, n = ( int )m_particles.size(); i < n; i++ ) {
		m_particles[i]->render( renderer );
	}
}

} // namespace particles

} // namespace dreemchest